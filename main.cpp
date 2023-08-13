#include <SFML/Graphics.hpp>
#include <cmrc/cmrc.hpp>
#include <magic_enum.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "configuration.hpp"
#include "inputmanager.hpp"
#include "rom.hpp"
#include "romsource.hpp"
#include "version.hpp"

CMRC_DECLARE(resources);

int main()
{
    // Loading log level from environment variable
    spdlog::cfg::load_env_levels();

    spdlog::info("Starting {} v{}", projectName, projectVersion);

    // Searching for advmame
    spdlog::info("Searching for advanceMAME on the system");
    if (std::system("advmame --version >/dev/null 2>&1") != 0)
    {
        spdlog::error("advanceMAME not found on the system");
        return 1;
    }

    // Scanning rom folder
    auto romPath = Configuration::get().romDirectory();
    spdlog::info("Scanning for roms in: {}", romPath.string());
    RomSource romSource(romPath);

    // Scan succesful
    std::vector<Rom> romList;
    if (auto scanResult = romSource.scan(); std::holds_alternative<std::vector<Rom>>(scanResult))
    {
        romList = std::get<std::vector<Rom>>(scanResult);

        if (romList.empty())
        {
            spdlog::warn("Scan successful but folder is empty");
        }

        else
        {
            for (const auto& rom : romList)
            {
                spdlog::debug("Found rom: {}", rom.name());
            }
        }
    }

    // Scan failed
    else
    {
        spdlog::error("Scanning folder failed: {}", magic_enum::enum_name(std::get<RomSource::Error>(scanResult)));
        return 1;
    }

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

    // Getting font
    auto embeddedFS = cmrc::resources::get_filesystem();
    sf::Font font;
    cmrc::file fontFile;
    try
    {
        fontFile = embeddedFS.open("fonts/inter.ttf");
    }
    catch (const std::system_error& exception)
    {
        spdlog::error("Loading font file failed: {}", exception.what());
        return 1;
    }
    font.loadFromMemory(fontFile.begin(), std::distance(fontFile.begin(), fontFile.end()));

    // Constructing drawable rom list
    std::vector<sf::Text> romMenu;
    unsigned int selected = 0;
    for (const auto& rom : romList)
    {
        auto& text = romMenu.emplace_back(rom.name(), font, 36);
        text.setFillColor(sf::Color::Red);
        text.setOrigin(text.getGlobalBounds().width / 2, 0);
    }
    if (!romMenu.empty())
    {
        romMenu.begin()->setFillColor(sf::Color::White);
    }

    // Constructing project name and version
    sf::Text versionText(std::string(projectName) + std::string(" v") + std::string(projectVersion), font, 16);
    constexpr unsigned int versionSpacing = 20;
    versionText.setFillColor(sf::Color::Red);
    versionText.setOrigin(versionText.getGlobalBounds().getSize().x, versionText.getGlobalBounds().getSize().y);
    versionText.setPosition(view.getSize().x - versionSpacing, view.getSize().y - versionSpacing);

    // Connecting signals
    InputManager::get().closeWindow.connect([&window]() { window.close(); });

    InputManager::get().goUp.connect([&selected]() {
        if (selected != 0)
            selected--;
    });

    InputManager::get().goDown.connect([&selected, &romMenu]() {
        if (selected < romMenu.size() - 1)
            selected++;
    });

    InputManager::get().select.connect([&selected, &romList, &romPath]() {
        std::string advMameCmd = "advmame";
        std::string advMameRom = romList[selected].name();

        std::stringstream cmd;
        cmd << advMameCmd << " --dir_rom " << romPath << " " << advMameRom;

        spdlog::info("Launching {} from {}", advMameRom, romPath.string());
        if (auto advMameError = std::system(cmd.str().c_str()) != 0)
        {
            spdlog::error("AdvanceMAME failed to launch with exit code {}", advMameError);
        }
    });

    while (window.isOpen())
    {
        InputManager::get().manage(window);

        window.clear();

        constexpr unsigned int menuRows = 15;
        constexpr unsigned int spacing = 16;
        const float topSpacing = view.getSize().y / 5;
        for (unsigned int i = (selected / menuRows) * menuRows;
             i < std::min(static_cast<std::size_t>((selected / menuRows) * menuRows + menuRows), romMenu.size()); i++)
        {
            auto yOffset = i % menuRows == 0
                               ? topSpacing
                               : romMenu[i - 1].getPosition().y + romMenu[i - 1].getGlobalBounds().height + spacing;
            romMenu[i].setPosition(view.getSize().x / 2, yOffset);
            i == selected ? romMenu[i].setFillColor(sf::Color::White) : romMenu[i].setFillColor(sf::Color::Red);
            window.draw(romMenu[i]);
        }
        window.draw(versionText);

        window.display();
    }

    spdlog::info("Stopping {} v{}", projectName, projectVersion);

    return 0;
}