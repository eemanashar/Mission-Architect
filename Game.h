#pragma once

#include "MissionArt.h"
#include "Simulation.h"
#include <memory>
#include "Button.h"
#include "DrawUtil.h"
#include "LaunchVehicle.h"
#include "Mission.h"

#include <SFML/Graphics.hpp>

#include <functional>
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    enum class Screen {
        MissionSelect,
        Design,
        Launch,
        Validation,
        Simulation,
        Results
    };

    enum class DesignTab {
        Structure,
        Power,
        Comms,
        Propulsion,
        Instrument
    };
    void startSimulation();
    void addSimulationButtons();
    void drawSimulation();
    void drawResults();
    void handle(const sf::Event& event);
    void onClick(sf::Vector2f point);
    void updateHover(sf::Vector2f point);
    void rebuild();
    void drawFrame();
    void drawMission();
    void drawDesign();
    void drawLaunch();
    void drawValidation();
    void drawMeters();
    void drawHeader(const std::string& title, const std::string& subtitle);
    void addButton(const std::string& label, sf::Vector2f position, sf::Vector2f size,
                   bool selected, std::function<void()> action, unsigned characterSize = 18);
    void reset();

    PartCategory tabCategory() const;
    const LaunchVehicle* launcher() const;
    ResourceReport report() const;

    float encounterTime_ = 0.f;
    std::unique_ptr<Simulation> sim_;
    sf::Clock animClock_;
    float shownProgress_ = 0.f;
    float animTime_ = 0.f;
    sf::RenderWindow window_;
    sf::Font font_;
    bool fontOk_ = false;
    std::vector<Star> stars_;
    std::vector<Mission> missions_;
    int missionIndex_ = 0;
    Spacecraft craft_;
    std::vector<LaunchVehicle> launchers_;
    int launchIndex_ = -1;
    Screen screen_ = Screen::MissionSelect;
    DesignTab tab_ = DesignTab::Structure;
    std::vector<Button> buttons_;
    std::vector<std::function<void()>> actions_;
};
