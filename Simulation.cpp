#include "Simulation.h"

#include "Systems.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {

    const double kSuccessFraction = 0.5;        
    const double kInstrumentFailChance = 0.08;

    double clampd(double v, double lo, double hi) { return std::max(lo, std::min(hi, v)); }

    std::string num(double value, int decimals = 0) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(decimals) << value;
        return out.str();
    }

    std::string pct(double fraction) { return num(fraction * 100.0, 0) + "%"; }

    double difficultyFactor(std::string text) {
        std::transform(text.begin(), text.end(), text.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (text.find("easy") != std::string::npos || text.find("beginner") != std::string::npos) return 0.8;
        if (text.find("hard") != std::string::npos || text.find("advanced") != std::string::npos ||
            text.find("expert") != std::string::npos) return 1.3;
        return 1.0;
    }

} 

Simulation::Simulation(const Mission& mission, const Spacecraft& craft,
    const LaunchVehicle& launcher, const ResourceReport& report)
    : rng_(std::random_device{}()), report_(report) {
    launcherName_ = launcher.name();
    destination_ = mission.destination();
    profile_ = profileFor(mission.name());
    difficulty_ = 1.0;  
    launchReliability_ = launcher.reliability();
    if (launchReliability_ > 1.0) launchReliability_ /= 100.0; 

    if (const PowerSystem* power = dynamic_cast<const PowerSystem*>(craft.power()))
        radioisotope_ = power->isRadioisotope();

    if (craft.propulsion() && report_.massKg > 0) {
        hasPropulsion_ = true;
        propFraction_ = craft.propulsion()->spec().massKg / report_.massKg;
    }

    for (const auto& part : craft.instruments()) {
        InstrumentState state{ part->name(), static_cast<double>(part->scienceValue()),
                              part->producedDataMbps(), true };
        sciencePossible_ += state.science;
        instruments_.push_back(state);
    }

    initialMargin_ = powerMargin();
    say(LogLevel::Info, "Mission '" + mission.name() + "' is ready on the pad. Press Next phase.");
}

const char* Simulation::phaseName(Phase phase) {
    switch (phase) {
    case Phase::Launch: return "Launch";
    case Phase::Cruise: return "Cruise";
    case Phase::OrbitInsertion: return "Orbit insertion";
    case Phase::Science: return "Science ops";
    case Phase::Downlink: return "Downlink";
    case Phase::Done: return "Done";
    }
    return "";
}


void Simulation::say(LogLevel level, const std::string& text) {
    LogEntry entry;
    entry.text = text;
    entry.level = level;
    log_.push_back(entry);
}

void Simulation::fail(const std::string& reason) {
    failed_ = true;
    reasons_.push_back(reason);
}

double Simulation::roll() {
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng_);
}

bool Simulation::chance(double probability) { return roll() < probability; }

double Simulation::powerMargin() const {
    if (report_.powerDrawW <= 0) return 1.0;
    return (report_.powerGenW * powerHealth_ - report_.powerDrawW) / report_.powerDrawW;
}

double Simulation::stress() const {
    const double m = powerMargin();
    if (m < 0.0) return 2.0;
    if (m < 0.10) return 1.7;
    if (m < 0.25) return 1.3;
    return 1.0;
}


void Simulation::step() {
    if (finished()) return;
    const Phase current = phase_;
    say(LogLevel::Info, std::string("--- ") + phaseName(current) + " ---");

    switch (current) {
    case Phase::Launch: runLaunch(); break;
    case Phase::Cruise: runCruise(); break;
    case Phase::OrbitInsertion: runInsertion(); break;
    case Phase::Science: runScience(); break;
    case Phase::Downlink: runDownlink(); break;
    case Phase::Done: return;
    }

    if (failed_) {
        failedPhase_ = static_cast<int>(current);
        phase_ = Phase::Done;
        finalise();
        return;
    }

    ++completed_;
    if (current == Phase::Downlink) {
        phase_ = Phase::Done;
        finalise();
    }
    else {
        phase_ = static_cast<Phase>(static_cast<int>(current) + 1);
    }
}

void Simulation::runLaunch() {
    if (report_.overMass()) {
        say(LogLevel::Bad, "The rocket cannot lift " + num(report_.massKg) + " kg (limit " +
            num(report_.capacityKg) + " kg).");
        fail("The spacecraft (" + num(report_.massKg) + " kg) was heavier than the " + launcherName_ +
            " can lift (" + num(report_.capacityKg) + " kg).");
        return;
    }

    double p = launchReliability_;
    const double load = report_.capacityKg > 0 ? report_.massKg / report_.capacityKg : 1.0;
    if (load > 0.95) {
        p -= 0.03;
        say(LogLevel::Warning, "Payload is at " + pct(load) + " of capacity. Ascent margin is thin.");
    }

    say(LogLevel::Info, "Ignition. " + launcherName_ + " lifts " + num(report_.massKg) + " kg.");
    if (!chance(p)) {
        say(LogLevel::Bad, "Launch vehicle failure during ascent.");
        fail("The " + launcherName_ + " failed on ascent. Its reliability is " + pct(launchReliability_) +
            "; a more reliable rocket lowers this risk.");
        return;
    }
    say(LogLevel::Good, "Orbit reached. Spacecraft separated cleanly.");
}

void Simulation::solarFlare() {
    say(LogLevel::Warning, "Solar flare! A radiation surge sweeps over the spacecraft.");
    if (radioisotope_) {
        say(LogLevel::Good, "The RTG does not depend on sunlight, so the surge does no harm.");
        return;
    }
    const double margin = powerMargin();
    if (margin >= 0.25) {
        say(LogLevel::Good, "A healthy " + pct(margin) + " power margin absorbed the surge.");
        return;
    }
    const double loss = margin >= 0.10 ? 0.10 : 0.25;
    powerHealth_ *= (1.0 - loss);
    ++problems_;
    say(LogLevel::Bad, "Solar arrays damaged: " + pct(loss) + " of generation lost.");
    reasons_.push_back("A solar flare cut power generation by " + pct(loss) +
        " because the power margin was only " + pct(margin) + ".");
}

void Simulation::runCruise() {
    say(LogLevel::Info, "Coasting toward " + destination_ + ". Power margin: " + pct(powerMargin()) + ".");
    if (chance(profile_.flareChance)) solarFlare();
    else say(LogLevel::Good, "Quiet cruise. All systems nominal.");
}

void Simulation::runInsertion() {
    say(LogLevel::Info, std::string(profile_.flyby ? "Course-correction burn for the flyby of "
        : "Orbit insertion burn at ") + destination_ + ".");
    const double needed = profile_.requiredPropFraction;
    double p = 0.15;
    if (hasPropulsion_) {
        const double ratio = needed > 0 ? propFraction_ / needed : 1.0;
        p = std::min(0.96, 0.55 + 0.41 * std::min(ratio, 1.0));
        say(LogLevel::Info, "Propulsion is " + pct(propFraction_) + " of spacecraft mass (about " +
            pct(needed) + " needed). Burn success chance " + pct(p) + ".");
    }
    else {
        say(LogLevel::Warning, "No propulsion system fitted. The burn is very unlikely to work.");
    }

    if (!chance(p)) {
        say(LogLevel::Bad, profile_.flyby ? "Trajectory error. The spacecraft missed its target."
            : "Burn fell short. The spacecraft missed capture and flew past.");
        fail(std::string(profile_.flyby ? "The flyby burn failed. " : "Orbit insertion failed. ") +
            "Propulsion was " + pct(propFraction_) + " of spacecraft mass; about " + pct(needed) +
            " was needed for a reliable burn.");
        return;
    }
    reachedDestination_ = true;
    say(LogLevel::Good, profile_.flyby ? "Flyby trajectory locked. Closest approach reached."
        : "Capture burn complete. Stable orbit achieved.");
}

void Simulation::runDownlink() {
    const double cap = report_.dataCapMbps * profile_.linkScale;
    say(LogLevel::Info, "Downlink open. Instruments produced " + num(dataProducedMbps_, 1) +
        " Mbps; the antenna carries " + num(cap, 1) + " Mbps.");
    if (profile_.linkScale < 1.0)
        say(LogLevel::Warning, "Distance weakens the link to " + pct(profile_.linkScale) + " of its rated speed.");

    double fraction = 1.0;
    if (dataProducedMbps_ > 0 && cap < dataProducedMbps_) fraction = cap / dataProducedMbps_;
    if (fraction < 1.0) {
        ++problems_;
        say(LogLevel::Warning, "Link too slow: only " + pct(fraction) + " of the data can be sent.");
        reasons_.push_back("The comms link (" + num(cap, 1) + " Mbps at this distance) could not carry the "
            "instruments' output (" + num(dataProducedMbps_, 1) + " Mbps), so " +
            pct(1.0 - fraction) + " of the data was never sent.");
    }

    double loss = 0.0;
    if (chance(profile_.dropoutChance)) {
        say(LogLevel::Warning, "Comms dropout!");
        const bool spare = dataProducedMbps_ <= 0 || cap >= 1.5 * dataProducedMbps_;
        if (spare) {
            say(LogLevel::Good, "Spare bandwidth let the link resend the lost packets.");
        }
        else {
            loss = 0.15 + 0.25 * roll();
            ++problems_;
            say(LogLevel::Bad, "Dropout lost " + pct(loss) + " of the data.");
            reasons_.push_back("A comms dropout cost " + pct(loss) +
                " of the data because there was no spare bandwidth to resend it.");
        }
    }

    scienceReturned_ = scienceCollected_ * fraction * (1.0 - loss);
    say(scienceReturned_ > 0 ? LogLevel::Good : LogLevel::Bad,
        "Downlink complete: " + num(scienceReturned_, 1) + " science points reached Earth.");
}
void Simulation::runScience() {
    say(LogLevel::Info, "Science operations begin at " + destination_ + ".");
    if (chance(0.20)) solarFlare();

    operational_ = 1.0;
    if (report_.powerDrawW > 0)
        operational_ = clampd(report_.powerGenW * powerHealth_ / report_.powerDrawW, 0.0, 1.0);

    if (operational_ < 1.0) {
        ++problems_;
        say(LogLevel::Warning, "Power short. Instruments throttled to " + pct(operational_) + ".");
        reasons_.push_back("Power generation (" + num(report_.powerGenW * powerHealth_) +
            " W) could not cover the " + num(report_.powerDrawW) +
            " W draw, so instruments ran at " + pct(operational_) + ".");
    }
    else {
        say(LogLevel::Good, "Power balanced. Instruments running at full rate.");
    }

    for (auto& inst : instruments_) {
        if (chance(kInstrumentFailChance * stress())) {
            inst.working = false;
            ++problems_;
            say(LogLevel::Bad, inst.name + " has failed.");
            reasons_.push_back(inst.name + " failed in orbit" +
                (stress() > 1.0 ? "; a thin power margin raises the failure rate." : "."));
        }
        else {
            say(LogLevel::Good, inst.name + " is collecting data.");
        }
    }

    scienceCollected_ = 0;
    dataProducedMbps_ = 0;
    for (const auto& inst : instruments_) {
        if (!inst.working) continue;
        scienceCollected_ += inst.science * operational_;
        dataProducedMbps_ += inst.dataMbps * operational_;
    }

    if (instruments_.empty()) say(LogLevel::Warning, "No instruments aboard. Nothing to measure.");
    else if (scienceCollected_ <= 0) say(LogLevel::Bad, "Every instrument is down.");
    else say(LogLevel::Info, "Collected " + num(scienceCollected_, 1) + " science points.");
}


// ---------- scoring ----------

void Simulation::finalise() {
    const double target = profile_.scienceTarget;
    const double cost = report_.totalCost();

    result_.reachedDestination = reachedDestination_;
    result_.sciencePossible = sciencePossible_;
    result_.scienceReturned = scienceReturned_;

    // Science: 50 points.
    result_.scoreScience = static_cast<int>(std::lround(50.0 * clampd(scienceReturned_ / target, 0.0, 1.0)));

    // Budget efficiency: 25 points. Science per dollar compared with the benchmark. Over budget scores 0.
    double efficiency = 0.0;
    if (!report_.overBudget() && cost > 0 && report_.budgetMillion > 0)
        efficiency = clampd((scienceReturned_ / cost) / (target / report_.budgetMillion), 0.0, 1.0);
    result_.scoreBudget = static_cast<int>(std::lround(25.0 * efficiency));

    // Reliability: 25 points. Phases completed, reduced by every problem along the way.
    const double clean = 1.0 - std::min(0.8, 0.2 * problems_);
    result_.scoreReliability = static_cast<int>(std::lround(25.0 * completed_ / kPhaseCount * clean));

    result_.success = !failed_ && scienceReturned_ >= kSuccessFraction * target;
    result_.headline = failed_ ? "MISSION LOST" : (result_.success ? "MISSION SUCCESS" : "PARTIAL SUCCESS");
    result_.reasons = reasons_;

    // Design decisions that mattered, whether or not they triggered an event.
    if (report_.overBudget())
        result_.reasons.push_back("You spent $" + num(cost) + "M against a $" + num(report_.budgetMillion) +
            "M budget, so the budget score is zero.");
    if (initialMargin_ < 0.10)
        result_.reasons.push_back("Your power margin was only " + pct(initialMargin_) +
            ". More generation or less consumption makes failures rarer and less damaging.");
    else if (initialMargin_ >= 0.25 && !failed_)
        result_.reasons.push_back("A comfortable power margin (" + pct(initialMargin_) +
            ") kept the spacecraft stable.");
    if (report_.capacityKg > 0 && report_.massKg / report_.capacityKg > 0.95)
        result_.reasons.push_back("The payload used " + pct(report_.massKg / report_.capacityKg) +
            " of the rocket's capacity, leaving little ascent margin.");
    if (!failed_) {
        if (scienceReturned_ >= target)
            result_.reasons.push_back("The instruments returned " + num(scienceReturned_, 1) +
                " science points, beating the target of " + num(target) + ".");
        else
            result_.reasons.push_back("Only " + num(scienceReturned_, 1) + " of the " + num(target) +
                " target science points came back. More instruments or a faster link would help.");
    }
    if (result_.reasons.empty()) result_.reasons.push_back("Nothing went wrong.");
}