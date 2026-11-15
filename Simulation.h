#pragma once

#include "LaunchVehicle.h"
#include "Mission.h"
#include "Spacecraft.h"
#include "MissionProfile.h"

#include <random>
#include <string>
#include <vector>

enum class Phase { Launch = 0, Cruise, OrbitInsertion, Science, Downlink, Done };
enum class LogLevel { Info, Good, Warning, Bad };

struct LogEntry {
    std::string text;
    LogLevel level = LogLevel::Info;
};

struct SimResult {
    bool success = false;
    bool reachedDestination = false;
    double scienceReturned = 0;
    double sciencePossible = 0;
    int scoreScience = 0;      
    int scoreBudget = 0;    
    int scoreReliability = 0;  
    std::string headline;
    std::vector<std::string> reasons;
    int total() const { return scoreScience + scoreBudget + scoreReliability; }
};

// Turn-based mission run. Call step() once per phase. No SFML in here.
class Simulation {
public:
    enum { kPhaseCount = 5 };

    Simulation(const Mission& mission, const Spacecraft& craft,
        const LaunchVehicle& launcher, const ResourceReport& report);

    void step();

    bool finished() const { return phase_ == Phase::Done; }
    bool failed() const { return failed_; }
    int phasesCompleted() const { return completed_; }
    int failedPhase() const { return failedPhase_; }
    const std::vector<LogEntry>& log() const { return log_; }
    const SimResult& result() const { return result_; }

    double powerMargin() const;
    double powerHealth() const { return powerHealth_; }

    static const char* phaseName(Phase phase);

private:
    struct InstrumentState {
        std::string name;
        double science;
        double dataMbps;
        bool working;
    };
    MissionProfile profile_;
    void runLaunch();
    void runCruise();
    void runInsertion();
    void runScience();
    void runDownlink();
    void solarFlare();
    void finalise();

    void say(LogLevel level, const std::string& text);
    void fail(const std::string& reason);
    bool chance(double probability);
    double roll();
    double stress() const;

    std::mt19937 rng_;
    ResourceReport report_;
    std::string launcherName_;
    std::string destination_;
    double launchReliability_ = 0.9;
    double difficulty_ = 1.0;
    double propFraction_ = 0;
    bool hasPropulsion_ = false;
    bool radioisotope_ = false;

    double powerHealth_ = 1.0;
    double initialMargin_ = 0;
    double operational_ = 1.0;
    double scienceCollected_ = 0;
    double dataProducedMbps_ = 0;
    double scienceReturned_ = 0;
    double sciencePossible_ = 0;
    std::vector<InstrumentState> instruments_;

    int problems_ = 0;
    int completed_ = 0;
    int failedPhase_ = -1;
    bool failed_ = false;
    bool reachedDestination_ = false;
    Phase phase_ = Phase::Launch;

    std::vector<LogEntry> log_;
    std::vector<std::string> reasons_;
    SimResult result_;
};