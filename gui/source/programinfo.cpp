#include "programinfo.hpp"

#include <memory>
#include <optional>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include <fmt/core.h>

#include "emulator.hpp"
#include "internalresourcemanager.hpp"
#include "softwareinfo.hpp"

ProgramInfo::ProgramInfo()
{
    auto softwareInfo = std::make_shared<TextNode>();
    softwareInfo->element().setFont(FontManager::get().getResource(FONT_PATH));
    softwareInfo->element().setCharacterSize(FONT_SIZE);
    softwareInfo->element().setString(fmt::format("{} v{}", projectName, projectVersion));
    softwareInfo->element().setFillColor(sf::Color::Red);
    addChild(softwareInfo);

    Emulator emulator;
    if (auto info = emulator.info(); info)
    {
        auto emulatorInfo = std::make_shared<TextNode>();
        emulatorInfo->element().setFont(FontManager::get().getResource(FONT_PATH));
        emulatorInfo->element().setCharacterSize(FONT_SIZE);
        emulatorInfo->element().setString(fmt::format("{} {}", info->name, info->version));
        emulatorInfo->element().setFillColor(sf::Color::Red);
        emulatorInfo->setPosition(0, softwareInfo->element().getGlobalBounds().height + SPACING);
        addChild(emulatorInfo);
    }
}
