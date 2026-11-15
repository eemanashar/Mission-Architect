#pragma once

#include "Spacecraft.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

struct Star {
    sf::Vector2f position;
    float radius = 1.f;
    sf::Color color;
};

std::vector<Star> makeStarfield(unsigned count, unsigned seed);

void drawStarfield(sf::RenderWindow& window, const std::vector<Star>& stars);

void drawText(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
              sf::Vector2f position, unsigned size, sf::Color color);

float drawWrapped(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
                  sf::Vector2f position, float width, unsigned size, sf::Color color);

void drawPanel(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size);

void drawMeter(sf::RenderWindow& window, const sf::Font& font, sf::Vector2f position, float width,
               const std::string& label, float fraction, sf::Color fill, const std::string& readout);

void drawSpacecraft(sf::RenderWindow& window, sf::Vector2f center, const Spacecraft& craft);

void drawMoon(sf::RenderWindow& window, sf::Vector2f center, float radius);
