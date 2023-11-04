#include "rominfo.hpp"

#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/String.hpp>

#include "resourcemanager.hpp"
#include "rommenu.hpp"

RomInfo::RomInfo(const RomMenu& romMenu) : mRom(romMenu.selectedRom())
{
    mSelectionConnection =
        romMenu.selectionChanged.connect([this](const std::optional<Rom>& rom) { this->selectionChanged(rom); });
    mFont = FontManager::get().getResource("fonts/inter.ttf");
}

void RomInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    std::scoped_lock lock(mMutex);
    if (mRom.has_value())
    {
        auto year = mRom->year().has_value() ? mRom->year().value() : "Unknown Year";
        auto manufacturer = mRom->manufacturer().has_value() ? mRom->manufacturer().value() : "Unknown Manufacturer";

        sf::Text text(year + std::string(", ") + manufacturer, mFont, 20);
        text.setFillColor(sf::Color::White);
        states.transform *= this->getTransform();
        target.draw(text, states);
    }
}