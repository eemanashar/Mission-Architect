#include "DrawUtil.h"

#include <algorithm>
#include <cstdint>
#include <random>
#include <sstream>

std::vector<Star> makeStarfield(unsigned count, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> x(0.f, 1280.f);
    std::uniform_real_distribution<float> y(0.f, 720.f);
    std::uniform_int_distribution<int> bright(140, 255);
    std::uniform_real_distribution<float> radius(0.6f, 1.8f);

    std::vector<Star> stars;
    stars.reserve(count);
    for (unsigned i = 0; i < count; ++i) {
        const auto shade = static_cast<std::uint8_t>(bright(rng));
        const auto blue = static_cast<std::uint8_t>(std::min(255, shade + 20));
        stars.push_back(Star{{x(rng), y(rng)}, radius(rng), sf::Color(shade, shade, blue)});
    }
    return stars;
}

void drawStarfield(sf::RenderWindow& window, const std::vector<Star>& stars) {
    for (const Star& star : stars) {
        sf::CircleShape dot(star.radius);
        dot.setOrigin({star.radius, star.radius});
        dot.setPosition(star.position);
        dot.setFillColor(star.color);
        window.draw(dot);
    }
}

void drawText(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
              sf::Vector2f position, unsigned size, sf::Color color) {
    sf::Text label(font, text, size);
    label.setFillColor(color);
    label.setPosition(position);
    window.draw(label);
}

float drawWrapped(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
                  sf::Vector2f position, float width, unsigned size, sf::Color color) {
    std::istringstream words(text);
    std::string word;
    std::string line;
    float y = position.y;
    sf::Text measure(font, "", size);
    const float step = static_cast<float>(size) + 6.f;

    while (words >> word) {
        const std::string trial = line.empty() ? word : line + " " + word;
        measure.setString(trial);
        if (measure.getLocalBounds().size.x > width && !line.empty()) {
            drawText(window, font, line, {position.x, y}, size, color);
            y += step;
            line = word;
        } else {
            line = trial;
        }
    }

    if (!line.empty()) {
        drawText(window, font, line, {position.x, y}, size, color);
        y += step;
    }
    return y;
}

void drawPanel(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size) {
    sf::RectangleShape panel(size);
    panel.setPosition(position);
    panel.setFillColor(sf::Color(14, 24, 42, 220));
    panel.setOutlineColor(sf::Color(48, 78, 112));
    panel.setOutlineThickness(2.f);
    window.draw(panel);
}

void drawMeter(sf::RenderWindow& window, const sf::Font& font, sf::Vector2f position, float width,
               const std::string& label, float fraction, sf::Color fill, const std::string& readout) {
    drawText(window, font, label, position, 14, sf::Color(186, 198, 214));

    const float barY = position.y + 20.f;
    sf::RectangleShape track({width, 14.f});
    track.setPosition({position.x, barY});
    track.setFillColor(sf::Color(20, 28, 42));
    track.setOutlineColor(sf::Color(60, 78, 102));
    track.setOutlineThickness(1.f);
    window.draw(track);

    const float clamped = std::max(0.f, std::min(fraction, 1.f));
    if (clamped > 0.f) {
        sf::RectangleShape bar({width * clamped, 14.f});
        bar.setPosition({position.x, barY});
        bar.setFillColor(fill);
        window.draw(bar);
    }

    drawText(window, font, readout, {position.x, barY + 18.f}, 13, fill);
}

namespace {

sf::Color instrumentColor(const std::string& id) {
    if (id == "inst.camera") return sf::Color(70, 190, 220);
    if (id == "inst.spec") return sf::Color(230, 180, 70);
    if (id == "inst.radar") return sf::Color(80, 200, 130);
    if (id == "inst.mag") return sf::Color(200, 120, 210);
    return sf::Color(180, 180, 180);
}

void drawSolarWing(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size) {
    sf::RectangleShape wing(size);
    wing.setPosition(position);
    wing.setFillColor(sf::Color(22, 58, 140));
    wing.setOutlineColor(sf::Color(110, 170, 255));
    wing.setOutlineThickness(1.f);
    window.draw(wing);

    const int cells = 3;
    const float gap = size.x / static_cast<float>(cells);
    for (int i = 1; i < cells; ++i) {
        sf::RectangleShape line({1.f, size.y});
        line.setPosition({position.x + gap * static_cast<float>(i), position.y});
        line.setFillColor(sf::Color(80, 130, 210));
        window.draw(line);
    }
}

} // namespace

void drawSpacecraft(sf::RenderWindow& window, sf::Vector2f center, const Spacecraft& craft) {
    float bodyW = 64.f;
    float panel = 0.f;
    float dish = 0.f;
    float nozzle = 16.f;
    bool hasBus = false;
    bool rtg = false;

    if (const Component* bus = craft.structure()) {
        hasBus = true;
        bodyW = bus->spec().visual;
    }
    if (const Component* power = craft.power()) {
        panel = power->spec().visual;
        rtg = panel <= 0.f;
    }
    if (const Component* comms = craft.comms())
        dish = comms->spec().visual;
    if (const Component* propulsion = craft.propulsion())
        nozzle = propulsion->spec().visual;

    const float bodyH = 92.f;
    const sf::Vector2f bodyPos{center.x - bodyW / 2.f, center.y - bodyH / 2.f};

    if (panel > 1.f) {
        drawSolarWing(window, {bodyPos.x - panel - 6.f, center.y - 14.f}, {panel, 28.f});
        drawSolarWing(window, {bodyPos.x + bodyW + 6.f, center.y - 14.f}, {panel, 28.f});
    }

    sf::ConvexShape nose(3);
    nose.setPoint(0, {bodyPos.x, bodyPos.y});
    nose.setPoint(1, {bodyPos.x + bodyW, bodyPos.y});
    nose.setPoint(2, {center.x, bodyPos.y - 26.f});
    nose.setFillColor(hasBus ? sf::Color(196, 206, 218) : sf::Color(70, 78, 92));
    window.draw(nose);

    sf::RectangleShape body({bodyW, bodyH});
    body.setPosition(bodyPos);
    body.setFillColor(hasBus ? sf::Color(168, 178, 194) : sf::Color(55, 62, 76));
    body.setOutlineColor(sf::Color(230, 236, 244));
    body.setOutlineThickness(2.f);
    window.draw(body);

    float boxY = bodyPos.y + 10.f;
    for (const auto& instrument : craft.instruments()) {
        sf::RectangleShape box({std::max(12.f, bodyW - 16.f), 12.f});
        box.setPosition({bodyPos.x + 8.f, boxY});
        box.setFillColor(instrumentColor(instrument->spec().id));
        window.draw(box);
        boxY += 16.f;
    }

    if (rtg) {
        sf::RectangleShape generator({18.f, 28.f});
        generator.setPosition({bodyPos.x + bodyW + 8.f, center.y + 18.f});
        generator.setFillColor(sf::Color(90, 78, 48));
        generator.setOutlineColor(sf::Color(210, 170, 70));
        generator.setOutlineThickness(1.f);
        window.draw(generator);
        for (int fin = 0; fin < 3; ++fin) {
            sf::RectangleShape vane({8.f, 2.f});
            vane.setPosition({bodyPos.x + bodyW + 26.f, center.y + 22.f + static_cast<float>(fin) * 8.f});
            vane.setFillColor(sf::Color(180, 150, 70));
            window.draw(vane);
        }
    }

    if (dish > 0.f) {
        sf::RectangleShape mast({3.f, 22.f});
        mast.setPosition({center.x + bodyW * 0.15f, bodyPos.y - 20.f});
        mast.setFillColor(sf::Color(200, 206, 214));
        window.draw(mast);

        sf::CircleShape antenna(dish);
        antenna.setOrigin({dish, dish});
        antenna.setPosition({center.x + bodyW * 0.15f + 1.f, bodyPos.y - 22.f});
        antenna.setFillColor(sf::Color(210, 216, 224));
        antenna.setOutlineColor(sf::Color(80, 90, 110));
        antenna.setOutlineThickness(2.f);
        window.draw(antenna);

        sf::CircleShape hub(std::max(2.f, dish * 0.28f));
        hub.setOrigin({hub.getRadius(), hub.getRadius()});
        hub.setPosition(antenna.getPosition());
        hub.setFillColor(sf::Color(70, 130, 160));
        window.draw(hub);
    }

    sf::RectangleShape engine({bodyW * 0.4f, nozzle});
    engine.setPosition({center.x - engine.getSize().x / 2.f, bodyPos.y + bodyH});
    engine.setFillColor(sf::Color(90, 96, 108));
    engine.setOutlineColor(sf::Color(180, 186, 196));
    engine.setOutlineThickness(1.f);
    window.draw(engine);

    if (craft.propulsion()) {
        sf::CircleShape flame(std::max(4.f, nozzle * 0.28f));
        flame.setOrigin({flame.getRadius(), 0.f});
        flame.setPosition({center.x, bodyPos.y + bodyH + nozzle});
        flame.setFillColor(sf::Color(255, 150, 60));
        window.draw(flame);
    }
}

void drawMoon(sf::RenderWindow& window, sf::Vector2f center, float radius) {
    sf::CircleShape moon(radius);
    moon.setOrigin({radius, radius});
    moon.setPosition(center);
    moon.setFillColor(sf::Color(186, 188, 196));
    moon.setOutlineColor(sf::Color(220, 224, 232));
    moon.setOutlineThickness(3.f);
    window.draw(moon);

    const sf::Vector2f craters[] = {
        {center.x - radius * 0.35f, center.y - radius * 0.2f},
        {center.x + radius * 0.25f, center.y - radius * 0.35f},
        {center.x + radius * 0.15f, center.y + radius * 0.3f},
        {center.x - radius * 0.1f, center.y + radius * 0.05f}
    };
    const float craterR[] = {radius * 0.18f, radius * 0.12f, radius * 0.16f, radius * 0.08f};
    for (int i = 0; i < 4; ++i) {
        sf::CircleShape crater(craterR[i]);
        crater.setOrigin({craterR[i], craterR[i]});
        crater.setPosition(craters[i]);
        crater.setFillColor(sf::Color(150, 152, 162));
        window.draw(crater);
    }
}
