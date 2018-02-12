#include "popup.hpp"

PopUp::PopUp() {
    //setting some default parameter so the popup is more or less ready to get drawn
    setSize(sf::Vector2f(680, 180));
    setFillColor(sf::Color::White);
    setOutlineColor(sf::Color::Black);
    setOutlineThickness(1.5);
    spacing = 50;
}

void PopUp::calculateRowPosition() {
    row.setTruncateWidth(getLocalBounds().width - spacing*2);
    row.setOrigin(row.getLocalBounds().width/2, row.getLocalBounds().height/2);
    row.setPosition(getLocalBounds().width/2, getLocalBounds().height/2);
}

void PopUp::calculateRowParameters() {
    row.setActive(isActive());
    row.setUpdateRate(getUpdateRate());
}

void PopUp::setRow(const Row& theRow) {
    detachAllChildren();
    row = theRow;
    calculateRowPosition();
    calculateRowParameters();
    attachChild(&row);
}
