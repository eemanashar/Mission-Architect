#include "Game.h"

#include "Catalog.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>

namespace {

sf::Vector2f mousePoint(sf::Vector2i pixel) {
    return {static_cast<float>(pixel.x), static_cast<float>(pixel.y)};
}

float optionTop(int index) {
    return 108.f + static_cast<float>(index) * 64.f;
}

float categoryTop(int index) {
    return 108.f + static_cast<float>(index) * 58.f;
}

std::string whole(double value) {
    return std::to_string(static_cast<int>(std::lround(value)));
}

std::string mbps(double value) {
    std::ostringstream text;
    text << std::fixed;
    text.precision(value < 1.0 ? 1 : 0);
    text << value;
    return text.str();
}

sf::Color meterColor(float ratio) {
    if (ratio < 0.75f)
        return sf::Color(70, 180, 110);
    if (ratio <= 1.f)
        return sf::Color(220, 180, 60);
    return sf::Color(200, 70, 70);
}

const sf::Color kTitle(230, 236, 245);
const sf::Color kMuted(150, 170, 190);
const sf::Color kBody(214, 222, 234);
const sf::Color kWarn(220, 120, 110);
const sf::Color kGood(110, 200, 150);

} 

Game::Game()
    : window_(sf::VideoMode({1280, 720}), "Mission Architect")
    , missions_(allMissions())
    , launchers_(availableLaunchers()) {
    window_.setFramerateLimit(60);
    fontOk_ = font_.openFromFile("C:/Windows/Fonts/arial.ttf");
    stars_ = makeStarfield(120, 15);
}

void Game::run() {
    if (!fontOk_) {
        std::cerr << "Could not open C:/Windows/Fonts/arial.ttf\n";
        window_.close();
        return;
    }

    rebuild();
    while (window_.isOpen()) {
        while (const std::optional event = window_.pollEvent())
            handle(*event);
        drawFrame();
    }
}

void Game::handle(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        window_.close();
        return;
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape)
            window_.close();
        return;
    }

    if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
        updateHover(mousePoint(moved->position));
        return;
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left)
            onClick(mousePoint(click->position));
    }
}

void Game::onClick(sf::Vector2f point) {
    for (std::size_t i = 0; i < buttons_.size(); ++i) {
        if (!buttons_[i].enabled() || !buttons_[i].contains(point))
            continue;
        actions_[i]();
        rebuild();
        return;
    }
}

void Game::updateHover(sf::Vector2f point) {
    for (Button& button : buttons_)
        button.setHovered(button.contains(point));
}

void Game::addButton(const std::string& label, sf::Vector2f position, sf::Vector2f size,
                     bool selected, std::function<void()> action, unsigned characterSize) {
    buttons_.emplace_back(font_, label, position, size, characterSize);
    buttons_.back().setSelected(selected);
    actions_.push_back(std::move(action));
}

void Game::reset() {
    craft_.clear();
    launchIndex_ = -1;
    missionIndex_ = 0;
    tab_ = DesignTab::Structure;
    screen_ = Screen::MissionSelect;
}

PartCategory Game::tabCategory() const {
    switch (tab_) {
    case DesignTab::Structure: return PartCategory::Structure;
    case DesignTab::Power: return PartCategory::Power;
    case DesignTab::Comms: return PartCategory::Comms;
    case DesignTab::Propulsion: return PartCategory::Propulsion;
    case DesignTab::Instrument: return PartCategory::Instrument;
    }
    return PartCategory::Structure;
}

const LaunchVehicle* Game::launcher() const {
    if (launchIndex_ < 0 || launchIndex_ >= static_cast<int>(launchers_.size()))
        return nullptr;
    return &launchers_[static_cast<std::size_t>(launchIndex_)];
}

ResourceReport Game::report() const {
    const double budget = missions_.empty() ? 0 : missions_[static_cast<std::size_t>(missionIndex_)].budgetMillion();
    return craft_.evaluate(budget, launcher());
}

void Game::rebuild() {
    buttons_.clear();
    actions_.clear();

    if (screen_ == Screen::MissionSelect) {
        for (int i = 0; i < static_cast<int>(missions_.size()); ++i) {
            addButton(missions_[static_cast<std::size_t>(i)].name(),
                      {48.f, 120.f + static_cast<float>(i) * 64.f}, {280.f, 52.f},
                      i == missionIndex_,
                      [this, i] { missionIndex_ = i; }, 20);
        }
        addButton("Design spacecraft", {980.f, 652.f}, {250.f, 46.f}, false,
                  [this] { screen_ = Screen::Design; }, 18);
        return;
    }

    if (screen_ == Screen::Design) {
        const DesignTab tabs[] = {
            DesignTab::Structure, DesignTab::Power, DesignTab::Comms,
            DesignTab::Propulsion, DesignTab::Instrument
        };
        const char* labels[] = {"Bus", "Power", "Comms", "Propulsion", "Instruments"};
        for (int i = 0; i < 5; ++i) {
            const DesignTab chosen = tabs[i];
            addButton(labels[i], {28.f, categoryTop(i)}, {190.f, 46.f}, tab_ == chosen,
                      [this, chosen] { tab_ = chosen; });
        }

        const PartCategory category = tabCategory();
        for (int i = 0; i < Catalog::count(category); ++i) {
            addButton(Catalog::spec(category, i).name,
                      {250.f, optionTop(i)}, {500.f, 32.f},
                      craft_.isSelected(category, i),
                      [this, category, i] { craft_.select(category, i); }, 16);
        }

        addButton("Back", {28.f, 652.f}, {160.f, 46.f}, false,
                  [this] { screen_ = Screen::MissionSelect; });
        addButton("Choose rocket", {1040.f, 652.f}, {210.f, 46.f}, false,
                  [this] { screen_ = Screen::Launch; });
        return;
    }

    if (screen_ == Screen::Launch) {
        for (int i = 0; i < static_cast<int>(launchers_.size()); ++i) {
            addButton(launchers_[static_cast<std::size_t>(i)].name(),
                      {48.f, 120.f + static_cast<float>(i) * 110.f}, {460.f, 40.f},
                      i == launchIndex_,
                      [this, i] { launchIndex_ = i; }, 18);
        }
        addButton("Back to design", {28.f, 652.f}, {210.f, 46.f}, false,
                  [this] { screen_ = Screen::Design; });
        addButton("Review mission", {1000.f, 652.f}, {240.f, 46.f}, false,
                  [this] { screen_ = Screen::Validation; });
        return;
    }

    if (screen_ == Screen::Validation) {
        addButton("Edit design", { 28.f, 652.f }, { 180.f, 46.f }, false,
            [this] { screen_ = Screen::Design; });
        addButton("Edit rocket", { 224.f, 652.f }, { 180.f, 46.f }, false,
            [this] { screen_ = Screen::Launch; });
        addButton("Start over", { 420.f, 652.f }, { 180.f, 46.f }, false,
            [this] { reset(); });
    }
    addSimulationButtons();

}

void Game::drawHeader(const std::string& title, const std::string& subtitle) {
    drawText(window_, font_, title, {36.f, 18.f}, 28, kTitle);
    drawText(window_, font_, subtitle, {36.f, 54.f}, 16, kMuted);
}

void Game::drawMeters() {
    const ResourceReport totals = report();
    const float y = 548.f;
    const float width = 270.f;

    const float budgetRatio = totals.budgetMillion <= 0
        ? 1.f
        : static_cast<float>(totals.totalCost() / totals.budgetMillion);
    drawMeter(window_, font_, {36.f, y}, width, "Budget", budgetRatio, meterColor(budgetRatio),
              "$" + whole(totals.totalCost()) + "M / $" + whole(totals.budgetMillion) + "M");

    if (!totals.hasLauncher()) {
        drawMeter(window_, font_, {344.f, y}, width, "Mass", 0.f, meterColor(1.f),
                  whole(totals.massKg) + " kg, no rocket yet");
    } else {
        const float massRatio = static_cast<float>(totals.massKg / totals.capacityKg);
        drawMeter(window_, font_, {344.f, y}, width, "Mass", massRatio, meterColor(massRatio),
                  whole(totals.massKg) + " / " + whole(totals.capacityKg) + " kg");
    }

    if (totals.powerGenW <= 0) {
        drawMeter(window_, font_, {652.f, y}, width, "Power", 1.f, meterColor(2.f), "no power system");
    } else {
        const float powerRatio = static_cast<float>(totals.powerDrawW / totals.powerGenW);
        drawMeter(window_, font_, {652.f, y}, width, "Power", powerRatio, meterColor(powerRatio),
                  whole(totals.powerDrawW) + " / " + whole(totals.powerGenW) + " W");
    }

    if (totals.dataCapMbps <= 0) {
        drawMeter(window_, font_, {960.f, y}, width, "Data", 1.f, meterColor(2.f), "no antenna");
    } else {
        const float dataRatio = static_cast<float>(totals.dataOutMbps / totals.dataCapMbps);
        drawMeter(window_, font_, {960.f, y}, width, "Data", dataRatio, meterColor(dataRatio),
                  mbps(totals.dataOutMbps) + " / " + mbps(totals.dataCapMbps) + " Mbps");
    }
}

void Game::drawMission() {
    drawHeader("Mission Architect", "Choose an objective.");

    if (missions_.empty())
        return;

    const Mission& mission = missions_[static_cast<std::size_t>(missionIndex_)];
    drawPanel(window_, {380.f, 120.f}, {860.f, 390.f});
    drawText(window_, font_, mission.name(), {408.f, 140.f}, 26, kTitle);

    float y = 186.f;
    const std::string lines[] = {
        "Destination: " + mission.destination(),
        "Science: " + mission.scienceGoals(),
        "Budget: $" + whole(mission.budgetMillion()) + "M",
        "Difficulty: " + mission.difficulty()
    };
    for (const std::string& line : lines) {
        drawText(window_, font_, line, {408.f, y}, 18, kBody);
        y += 32.f;
    }
    drawWrapped(window_, font_, mission.briefing(), {408.f, y + 8.f}, 800.f, 16, kMuted);
    drawBody(window_, {1120.f, 300.f}, 78.f, missions_[missionIndex_].name());
}

void Game::drawDesign() {
    const Mission& mission = missions_[static_cast<std::size_t>(missionIndex_)];
    drawHeader(mission.name(), "Pick one bus, power system, antenna, and engine. Instruments can stack.");

    drawPanel(window_, {780.f, 100.f}, {460.f, 420.f});
    drawText(window_, font_, "Spacecraft", {960.f, 112.f}, 16, kMuted);
    drawSpacecraft(window_, {1010.f, 310.f}, craft_);

    const PartCategory category = tabCategory();
    for (int i = 0; i < Catalog::count(category); ++i) {
        const std::unique_ptr<Component> part = Catalog::make(category, i);
        drawText(window_, font_, part->statsLine(), {250.f, optionTop(i) + 34.f}, 14, kMuted);
    }

    float detailY = 400.f;
    bool any = false;
    for (int i = 0; i < Catalog::count(category); ++i) {
        if (!craft_.isSelected(category, i))
            continue;
        any = true;
        const std::unique_ptr<Component> part = Catalog::make(category, i);
        detailY = drawWrapped(window_, font_, part->detailLine(), {250.f, detailY}, 500.f, 15, kBody);
    }
    if (!any) {
        const char* hint = category == PartCategory::Instrument
            ? "Click an instrument to add it. Click it again to take it off."
            : "Click a part. Cost, mass, power, and data update in the bars.";
        drawWrapped(window_, font_, hint, {250.f, 400.f}, 500.f, 15, kMuted);
    }

    drawMeters();
}

void Game::drawLaunch() {
    drawHeader("Launch vehicle", "Payload capacity, price, and reliability. Mass over the limit is a red bar.");

    for (int i = 0; i < static_cast<int>(launchers_.size()); ++i) {
        const LaunchVehicle& rocket = launchers_[static_cast<std::size_t>(i)];
        const float top = 120.f + static_cast<float>(i) * 110.f;
        drawText(window_, font_, rocket.statsLine(), {48.f, top + 46.f}, 15, kBody);
        drawWrapped(window_, font_, rocket.blurb(), {48.f, top + 68.f}, 620.f, 14, kMuted);
    }

    drawPanel(window_, {780.f, 120.f}, {460.f, 390.f});
    drawSpacecraft(window_, {1010.f, 320.f}, craft_);

    const ResourceReport totals = report();
    if (launcher() && totals.overMass()) {
        drawText(window_, font_, "Too heavy for this rocket. Remove parts or pick a larger one.",
                 {48.f, 470.f}, 16, kWarn);
    } else if (!launcher()) {
        drawText(window_, font_, "Select a rocket to compare its capacity with your mass.",
                 {48.f, 470.f}, 16, kMuted);
    } else {
        drawText(window_, font_, "Mass fits this rocket.", {48.f, 470.f}, 16, kGood);
    }

    drawMeters();
}

void Game::drawValidation() {
    const Mission& mission = missions_[static_cast<std::size_t>(missionIndex_)];
    drawHeader("Design check", mission.name() + "  -  $" + whole(mission.budgetMillion()) + "M budget");

    drawPanel(window_, {36.f, 100.f}, {700.f, 420.f});

    auto lineFor = [](const char* label, const Component* part) {
        std::string text = label;
        text += part ? part->name() : "not chosen";
        return text;
    };

    float y = 118.f;
    const std::string rows[] = {
        lineFor("Bus: ", craft_.structure()),
        lineFor("Power: ", craft_.power()),
        lineFor("Comms: ", craft_.comms()),
        lineFor("Propulsion: ", craft_.propulsion())
    };
    for (const std::string& row : rows) {
        drawText(window_, font_, row, {56.f, y}, 18, kBody);
        y += 28.f;
    }

    std::string instruments = "Instruments: ";
    if (craft_.instruments().empty()) {
        instruments += "none";
    } else {
        for (std::size_t i = 0; i < craft_.instruments().size(); ++i) {
            if (i > 0)
                instruments += ", ";
            instruments += craft_.instruments()[i]->name();
        }
    }
    drawText(window_, font_, instruments, {56.f, y}, 18, kBody);
    y += 28.f;

    const char* rocketName = launcher() ? launcher()->name().c_str() : "not chosen";
    drawText(window_, font_, std::string("Rocket: ") + rocketName, {56.f, y}, 18, kBody);
    y += 36.f;

    const ResourceReport totals = report();
    drawText(window_, font_, "Science value so far: " + std::to_string(totals.science),
             {56.f, y}, 16, kMuted);
    y += 28.f;

    if (totals.fits()) {
        y = drawWrapped(window_, font_,
                        "Budget, mass, power, and downlink all fit. Flight simulation is the next stage.",
                        {56.f, y}, 660.f, 16, kGood);
    } else {
        if (!totals.hasLauncher())
            y = drawWrapped(window_, font_, "No launch vehicle selected.", {56.f, y}, 660.f, 16, kWarn);
        if (totals.overBudget())
            y = drawWrapped(window_, font_, "Over budget. Drop a part or a cheaper rocket.", {56.f, y}, 660.f, 16, kWarn);
        if (totals.overMass())
            y = drawWrapped(window_, font_, "Over the rocket's payload capacity.", {56.f, y}, 660.f, 16, kWarn);
        if (totals.powerShort())
            y = drawWrapped(window_, font_, "Power draw is above what the generator can supply.", {56.f, y}, 660.f, 16, kWarn);
        if (totals.dataShort())
            y = drawWrapped(window_, font_, "Instruments produce more data than the antenna can send.", {56.f, y}, 660.f, 16, kWarn);
    }

    drawPanel(window_, {760.f, 100.f}, {480.f, 420.f});
    drawSpacecraft(window_, {1000.f, 310.f}, craft_);
    drawMeters();
}

void Game::drawFrame() {
    window_.clear(sf::Color(8, 12, 28));
    drawStarfield(window_, stars_);

    switch (screen_) {
    case Screen::MissionSelect: drawMission(); break;
    case Screen::Design: drawDesign(); break;
    case Screen::Launch: drawLaunch(); break;
    case Screen::Validation: drawValidation(); break;
    case Screen::Simulation: drawSimulation(); break;
    case Screen::Results:    drawResults();    break;
    }

    for (const Button& button : buttons_)
        button.draw(window_);

    window_.display();
}
