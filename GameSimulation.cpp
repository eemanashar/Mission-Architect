#include "Game.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>

namespace {

    const float kPi = 3.14159265f;
    const sf::Vector2f kEarth(150.f, 330.f);
    const sf::Vector2f kMoon(690.f, 290.f);

    sf::Color levelColor(LogLevel level) {
        switch (level) {
        case LogLevel::Good: return sf::Color(110, 220, 140);
        case LogLevel::Warning: return sf::Color(240, 200, 90);
        case LogLevel::Bad: return sf::Color(240, 100, 100);
        default: return sf::Color(200, 212, 235);
        }
    }

    sf::Color meterColor(float f) {
        if (f >= 0.6f) return sf::Color(70, 190, 110);
        if (f >= 0.3f) return sf::Color(230, 190, 70);
        return sf::Color(220, 80, 80);
    }

    sf::Vector2f pathPoint(float t) {
        const sf::Vector2f from(205.f, 315.f);
        const sf::Vector2f to(630.f, 290.f);
        sf::Vector2f p = from + (to - from) * t;
        p.y -= std::sin(t * kPi) * 70.f;
        return p;
    }

    float stopFor(int completed) {
        static const float stops[] = { 0.f, 0.10f, 0.72f, 0.92f, 0.92f, 0.92f };
        return stops[std::min(completed, 5)];
    }

    std::string whole(double v) { return std::to_string(static_cast<long long>(std::llround(v))); }

} 

void Game::startSimulation() {
    const LaunchVehicle* lv = launcher();
    if (!lv || !report().fits()) return;
    sim_ = std::make_unique<Simulation>(missions_[missionIndex_], craft_, *lv, report());
    shownProgress_ = 0.f;
    animTime_ = 0.f;
    encounterTime_ = 0.f;
    animClock_.restart();
    screen_ = Screen::Simulation;
    rebuild();  
}

void Game::addSimulationButtons() {
    if (screen_ == Screen::Validation) {
        addButton("Launch mission", sf::Vector2f(1010.f, 640.f), sf::Vector2f(230.f, 54.f), false,
            [this]() { startSimulation(); });
        buttons_.back().setEnabled(report().fits());
    }
    else if (screen_ == Screen::Simulation) {
        const bool done = sim_ && sim_->finished();
        addButton(done ? "View results" : "Next phase", sf::Vector2f(1010.f, 640.f),
            sf::Vector2f(230.f, 54.f), false, [this]() {
                if (!sim_) return;
                if (sim_->finished()) screen_ = Screen::Results;
                else sim_->step();
                rebuild();  
            });
    }
    else if (screen_ == Screen::Results) {
        addButton("Play again", sf::Vector2f(1010.f, 640.f), sf::Vector2f(230.f, 54.f), false, [this]() {
            reset();
            sim_.reset();
            screen_ = Screen::MissionSelect;
            rebuild();   
            });
    }
}

void Game::drawSimulation() {
    if (!sim_) return;
    const Mission& mission = missions_[missionIndex_];
    drawHeader("Mission simulation", mission.name() + " to " + mission.destination());

    const float dt = animClock_.restart().asSeconds();
    animTime_ += dt;

    drawPanel(window_, sf::Vector2f(40.f, 110.f), sf::Vector2f(760.f, 330.f));

    sf::CircleShape earth(45.f);
    earth.setOrigin(sf::Vector2f(45.f, 45.f));
    earth.setPosition(kEarth);
    earth.setFillColor(sf::Color(40, 90, 170));
    earth.setOutlineThickness(2.f);
    earth.setOutlineColor(sf::Color(120, 170, 240));
    window_.draw(earth);

    sf::CircleShape land(15.f);
    land.setOrigin(sf::Vector2f(15.f, 15.f));
    land.setPosition(kEarth + sf::Vector2f(-12.f, -10.f));
    land.setFillColor(sf::Color(60, 150, 90));
    window_.draw(land);

    drawBody(window_, kMoon, 40.f, mission.name());
    drawText(window_, font_, mission.destination(), sf::Vector2f(kMoon.x - 50.f, kMoon.y + 55.f), 15, sf::Color(200, 212, 235));

    for (int i = 0; i <= 28; ++i) {
        sf::CircleShape dot(2.f);
        dot.setOrigin(sf::Vector2f(2.f, 2.f));
        dot.setPosition(pathPoint(static_cast<float>(i) / 28.f));
        dot.setFillColor(sf::Color(70, 95, 140));
        window_.draw(dot);
    }

    const int completed = sim_->phasesCompleted();
    shownProgress_ += (stopFor(completed) - shownProgress_) * std::min(1.f, dt * 2.5f);
    sf::Vector2f pos = pathPoint(shownProgress_);
    const MissionProfile profile = profileFor(mission.name());
    if (completed >= 3) encounterTime_ += dt;
    if (completed >= 3 && !sim_->failed() && shownProgress_ > 0.85f) {
        const float a = profile.flyby ? kPi + std::min(kPi, encounterTime_ * 0.8f) : animTime_ * 1.6f;
        pos = kMoon + sf::Vector2f(std::cos(a), std::sin(a)) * 72.f;
    }

    sf::CircleShape craft(9.f, 3);
    craft.setOrigin(sf::Vector2f(9.f, 9.f));
    craft.setPosition(pos);
    craft.setFillColor(sim_->failed() ? sf::Color(230, 80, 80) : sf::Color(230, 236, 255));
    window_.draw(craft);
    if (sim_->failed())
        drawText(window_, font_, "SIGNAL LOST", sf::Vector2f(pos.x - 45.f, pos.y - 34.f), 16,
            sf::Color(240, 100, 100));

    for (int i = 0; i < Simulation::kPhaseCount; ++i) {
        const float x = 44.f + i * 152.f;
        sf::Color fill(28, 48, 78);
        if (i < completed) fill = sf::Color(40, 120, 80);
        else if (sim_->failed() && i == sim_->failedPhase()) fill = sf::Color(150, 50, 50);
        else if (i == completed && !sim_->finished()) fill = sf::Color(46, 110, 170);

        sf::RectangleShape box(sf::Vector2f(144.f, 40.f));
        box.setPosition(sf::Vector2f(x, 460.f));
        box.setFillColor(fill);
        box.setOutlineThickness(1.f);
        box.setOutlineColor(sf::Color(90, 130, 190));
        window_.draw(box);
        drawText(window_, font_, Simulation::phaseName(static_cast<Phase>(i)),
            sf::Vector2f(x + 10.f, 471.f), 15, sf::Color(230, 238, 255));
    }

    const float health = static_cast<float>(sim_->powerHealth());
    drawMeter(window_, font_, sf::Vector2f(44.f, 530.f), 360.f, "Power health", health,
        meterColor(health), std::to_string(static_cast<int>(health * 100.f + 0.5f)) + "%");

    const double margin = sim_->powerMargin();
    const float marginFill = static_cast<float>(std::max(0.0, std::min(1.0, margin / 0.5)));
    const sf::Color marginColor = margin >= 0.25 ? sf::Color(70, 190, 110)
        : margin >= 0.10 ? sf::Color(230, 190, 70) : sf::Color(220, 80, 80);
    drawMeter(window_, font_, sf::Vector2f(430.f, 530.f), 360.f, "Power margin", marginFill, marginColor,
        std::to_string(static_cast<int>(std::lround(margin * 100.0))) + "%");

    drawPanel(window_, sf::Vector2f(830.f, 110.f), sf::Vector2f(410.f, 500.f));
    drawText(window_, font_, "Mission log", sf::Vector2f(850.f, 120.f), 18, sf::Color(230, 238, 255));

    const auto& log = sim_->log();
    const int maxLines = 20;
    int lines = 0;
    std::size_t first = log.size();
    while (first > 0) {
        const int need = static_cast<int>(log[first - 1].text.size() / 44) + 1;
        if (lines + need > maxLines) break;
        lines += need;
        --first;
    }
    float y = 155.f;
    for (std::size_t i = first; i < log.size(); ++i)
        y = drawWrapped(window_, font_, log[i].text, sf::Vector2f(850.f, y), 370.f, 15,
            levelColor(log[i].level)) + 3.f;
}

void Game::drawResults() {
    if (!sim_) return;
    const SimResult& r = sim_->result();
    const Mission& mission = missions_[missionIndex_];
    const ResourceReport rep = report();
    drawHeader("Mission results", mission.name());

    drawPanel(window_, sf::Vector2f(40.f, 110.f), sf::Vector2f(520.f, 500.f));
    drawPanel(window_, sf::Vector2f(580.f, 110.f), sf::Vector2f(660.f, 500.f));

    const sf::Color headColor = sim_->failed() ? sf::Color(240, 100, 100)
        : r.success ? sf::Color(110, 220, 140) : sf::Color(240, 200, 90);
    drawText(window_, font_, r.headline, sf::Vector2f(70.f, 130.f), 36, headColor);
    drawText(window_, font_, "Total score: " + std::to_string(r.total()) + " / 100",
        sf::Vector2f(70.f, 185.f), 26, sf::Color(230, 238, 255));

    const float sci = r.scoreScience / 50.f;
    const float bud = r.scoreBudget / 25.f;
    const float rel = r.scoreReliability / 25.f;
    drawMeter(window_, font_, sf::Vector2f(70.f, 250.f), 460.f, "Science returned", sci, meterColor(sci),
        std::to_string(r.scoreScience) + " / 50");
    drawMeter(window_, font_, sf::Vector2f(70.f, 320.f), 460.f, "Budget efficiency", bud, meterColor(bud),
        std::to_string(r.scoreBudget) + " / 25");
    drawMeter(window_, font_, sf::Vector2f(70.f, 390.f), 460.f, "Reliability", rel, meterColor(rel),
        std::to_string(r.scoreReliability) + " / 25");

    drawText(window_, font_, "Science returned: " + whole(r.scienceReturned) + " of " +
        whole(r.sciencePossible) + " possible points",
        sf::Vector2f(70.f, 470.f), 16, sf::Color(200, 212, 235));
    drawText(window_, font_, "Cost: $" + whole(rep.totalCost()) + "M of $" + whole(rep.budgetMillion) + "M",
        sf::Vector2f(70.f, 500.f), 16, sf::Color(200, 212, 235));

    drawText(window_, font_, "Why it turned out this way", sf::Vector2f(605.f, 130.f), 22,
        sf::Color(230, 238, 255));
    float y = 175.f;
    for (const std::string& reason : r.reasons) {
        y = drawWrapped(window_, font_, "- " + reason, sf::Vector2f(605.f, y), 610.f, 16,
            sf::Color(200, 212, 235)) + 10.f;
        if (y > 570.f) break;
    }
}