#include "MissionArt.h"

#include "DrawUtil.h"
#include "MissionProfile.h"

namespace {

    void disc(sf::RenderWindow& window, sf::Vector2f center, float radius, sf::Color fill) {
        sf::CircleShape shape(radius);
        shape.setOrigin(sf::Vector2f(radius, radius));
        shape.setPosition(center);
        shape.setFillColor(fill);
        window.draw(shape);
    }

}  

void drawBody(sf::RenderWindow& window, sf::Vector2f center, float radius, const std::string& missionName) {
    const float r = radius;
    switch (profileFor(missionName).body) {
    case Body::Mars:
        disc(window, center, r, sf::Color(190, 90, 55));
        disc(window, center + sf::Vector2f(-0.35f * r, 0.15f * r), 0.28f * r, sf::Color(150, 65, 40));
        disc(window, center + sf::Vector2f(0.30f * r, -0.20f * r), 0.20f * r, sf::Color(160, 72, 45));
        disc(window, center + sf::Vector2f(0.f, -0.80f * r), 0.16f * r, sf::Color(235, 235, 245));
        break;

    case Body::Asteroid:
        disc(window, center, 0.70f * r, sf::Color(122, 116, 110));
        disc(window, center + sf::Vector2f(-0.45f * r, -0.20f * r), 0.42f * r, sf::Color(112, 106, 100));
        disc(window, center + sf::Vector2f(0.45f * r, 0.25f * r), 0.38f * r, sf::Color(128, 122, 116));
        disc(window, center + sf::Vector2f(0.10f * r, -0.15f * r), 0.12f * r, sf::Color(85, 80, 76));
        disc(window, center + sf::Vector2f(-0.30f * r, 0.30f * r), 0.09f * r, sf::Color(85, 80, 76));
        break;

    case Body::EarthOrbit: {
        sf::CircleShape ring(r);
        ring.setOrigin(sf::Vector2f(r, r));
        ring.setPosition(center);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(3.f);
        ring.setOutlineColor(sf::Color(90, 150, 230));
        window.draw(ring);
        disc(window, center, 0.35f * r, sf::Color(40, 90, 170));
        break;
    }

    default:
        drawMoon(window, center, radius);
        break;
    }
}