#include "menu.hpp"

Menu::Menu() {
    //setting some default parameter so the popup is more or less ready to get drawn
    setSize(sf::Vector2f(400, 380));
    setFillColor(sf::Color::White);
    setOutlineColor(sf::Color::Black);
    setOutlineThickness(1.5);
    spacing = 30;
}

void Menu::calculateListPosition() {
    list.setTruncateWidth(getLocalBounds().width - spacing*2);
    list.setTruncateHeight(getLocalBounds().height - spacing*2);
    list.setPosition(spacing, spacing);
}

void Menu::calculateListParameters() {
    list.setActive(isActive());
    list.setUpdateRate(getUpdateRate());
}

void Menu::setList(const List& theList) {
    detachAllChildren();
    list = theList;
    calculateListPosition();
    calculateListParameters();
    attachChild(&list);
}
