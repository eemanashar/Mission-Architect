#include "Button.h"

namespace {

sf::Color normalFill() { return sf::Color(28, 48, 78); }
sf::Color hoverFill() { return sf::Color(46, 78, 120); }
sf::Color selectedFill() { return sf::Color(24, 96, 118); }
sf::Color selectedHoverFill() { return sf::Color(32, 120, 142); }
sf::Color disabledFill() { return sf::Color(32, 36, 48); }
sf::Color outlineIdle() { return sf::Color(70, 100, 140); }
sf::Color outlineOn() { return sf::Color(120, 210, 220); }

}

Button::Button(const sf::Font& font, const std::string& label,
               sf::Vector2f position, sf::Vector2f size, unsigned characterSize)
    : shape_(size)
    , label_(font, label, characterSize) {
    shape_.setPosition(position);
    shape_.setOutlineThickness(2.f);
    label_.setFillColor(sf::Color(232, 238, 246));
    centerLabel();
    refreshColor();
}

void Button::setHovered(bool hovered) {
    hovered_ = hovered;
    refreshColor();
}

void Button::setSelected(bool selected) {
    selected_ = selected;
    refreshColor();
}

void Button::setEnabled(bool enabled) {
    enabled_ = enabled;
    label_.setFillColor(enabled ? sf::Color(232, 238, 246) : sf::Color(140, 146, 158));
    refreshColor();
}

bool Button::contains(sf::Vector2f point) const {
    return shape_.getGlobalBounds().contains(point);
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(shape_);
    window.draw(label_);
}

void Button::refreshColor() {
    if (!enabled_)
        shape_.setFillColor(disabledFill());
    else if (selected_ && hovered_)
        shape_.setFillColor(selectedHoverFill());
    else if (selected_)
        shape_.setFillColor(selectedFill());
    else if (hovered_)
        shape_.setFillColor(hoverFill());
    else
        shape_.setFillColor(normalFill());

    shape_.setOutlineColor(selected_ ? outlineOn() : outlineIdle());
}

void Button::centerLabel() {
    const sf::FloatRect bounds = label_.getLocalBounds();
    label_.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });
    const sf::Vector2f position = shape_.getPosition();
    const sf::Vector2f size = shape_.getSize();
    label_.setPosition({position.x + size.x / 2.f, position.y + size.y / 2.f});
}
