#include <SFML/Graphics.hpp>
#include <cmrc/cmrc.hpp>
#include <magic_enum.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "configuration.hpp"
#include "emulator.hpp"
#include "inputmanager.hpp"
#include "resourcemanager.hpp"
#include "rom.hpp"
#include "rominfo.hpp"
#include "rommenu.hpp"
#include "romsource.hpp"
#include "version.hpp"

int main()
{
    // Loading log level from environment variable
    spdlog::cfg::load_env_levels();

    spdlog::info("Starting {} v{}", projectName, projectVersion);

    // Searching for advmame
    spdlog::info("Searching for advanceMAME on the system");
    std::optional<std::string> emulatorVersion;
    if (emulatorVersion = Emulator::get().version(); !emulatorVersion.has_value())
    {
        spdlog::error("advanceMAME not found on the system");
        return 1;
    }

    // Constructing rom source
    auto romPath = Configuration::get().romDirectory();

    std::error_code createFolderEc;
    std::filesystem::create_directories(romPath, createFolderEc);
    if (createFolderEc)
    {
        spdlog::warn("Rom folder creation failed at {}", romPath.string());
    }

    RomSource romSource(romPath);
    romSource.romAdded.connect([](const Rom& rom) { spdlog::debug("Found rom: {}", rom.name()); });

    // Searching for a suitable video mode
    auto availableVideoModes = sf::VideoMode::getFullscreenModes();
    if (availableVideoModes.empty())
    {
        spdlog::error("No suitable screen resolution found");
        return 1;
    }
    auto videoMode = availableVideoModes[0];

    // Creating the window
    spdlog::info("Creating application window with mode: {}x{}x{}", videoMode.width, videoMode.height,
                 videoMode.bitsPerPixel);
    sf::RenderWindow window(availableVideoModes[0], std::string(projectName), sf::Style::Fullscreen);
    window.setMouseCursorVisible(false);
    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    // Scanning rom folder
    spdlog::info("Scanning for roms in: {}", romPath.string());

    if (auto ec = romSource.monitor(); ec.has_value())
    {
        spdlog::error("Error while scanning rom in {} Error: {}", romPath.string(), magic_enum::enum_name(ec.value()));
        return 1;
    }

    // Constructing drawable rom list
    RomMenu romMenu(romSource);
    romMenu.setPosition(view.getSize().x / 7, view.getSize().y / 6);

    // Constructing drawable rom info
    RomInfo romInfo{romMenu};
    romInfo.setPosition(view.getSize().x / 1.75f, view.getSize().y / 2.5f);

    // Constructing project name and version
    sf::Text versionText(std::string(projectName) + std::string(" v") + std::string(projectVersion),
                         FontManager::get().getResource("fonts/inter.ttf"), 16);
    constexpr unsigned int versionSpacing = 20;
    versionText.setFillColor(sf::Color::Red);
    versionText.setPosition(versionSpacing, view.getSize().y / 1.05f);

    // Constructing emulator name and version
    sf::Text emulatorText(emulatorVersion.value(), FontManager::get().getResource("fonts/inter.ttf"), 16);
    constexpr unsigned int emulatorSpacing = 5;
    emulatorText.setFillColor(sf::Color::Red);
    emulatorText.setPosition(versionText.getPosition().x,
                             versionText.getPosition().y + versionText.getGlobalBounds().getSize().y + emulatorSpacing);

    // Constructing "No Rom Found" text
    sf::Text noRomText("No rom found", FontManager::get().getResource("fonts/inter.ttf"), 24);
    noRomText.setFillColor(sf::Color::Red);
    noRomText.setOrigin(noRomText.getGlobalBounds().getSize().x / 2, noRomText.getGlobalBounds().getSize().y / 2);
    noRomText.setPosition(view.getSize().x / 2, view.getSize().y / 2);

    // Connecting signals
    InputManager::get().closeWindow.connect([&window]() { window.close(); });
    InputManager::get().goUp.connect([&romMenu]() { [[maybe_unused]] auto result = romMenu.selectedUp(); });
    InputManager::get().goDown.connect([&romMenu]() { [[maybe_unused]] auto result = romMenu.selectedDown(); });
    InputManager::get().select.connect([&romMenu]() {
        if (auto rom = romMenu.selectedRom(); rom.has_value())
        {
            if (auto error = rom.value().launch(); error.has_value())
            {
                spdlog::error("Failed to launch {}. Error: {}", rom.value().name(),
                              magic_enum::enum_name(error.value()));
            }
        }
    });

    while (window.isOpen())
    {
        InputManager::get().manage(window);

        window.clear();

        if (romSource.romList().empty())
        {
            window.draw(noRomText);
        }
        else
        {
            window.draw(romMenu);
            window.draw(romInfo);
        }

        window.draw(versionText);
        window.draw(emulatorText);

        window.display();
    }

    spdlog::info("Stopping {} v{}", projectName, projectVersion);

    return 0;
}