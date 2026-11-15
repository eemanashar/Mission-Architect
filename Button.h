#pragma once

#include <SFML/Graphics.hpp>

#include <string>

class Button {
public:
    Button(const sf::Font& font, const std::string& label,
           sf::Vector2f position, sf::Vector2f size, unsigned characterSize = 18);

    void setHovered(bool hovered);
    void setSelected(bool selected);
    void setEnabled(bool enabled);

    bool contains(sf::Vector2f point) const;
    bool enabled() const { return enabled_; }

    void draw(sf::RenderWindow& window) const;

private:
    void refreshColor();
    void centerLabel();

    sf::RectangleShape shape_;
    sf::Text label_;
    bool hovered_ = false;
    bool selected_ = false;
    bool enabled_ = true;
};
