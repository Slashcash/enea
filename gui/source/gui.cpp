#include "gui.hpp"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics.hpp>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "inputdevice.hpp"
#include "inputmanager.hpp"
#include "programinfo.hpp"
#include "rommenu.hpp"
#include "softwareinfo.hpp"

Gui::Gui(const std::vector<Rom>& romList) : mRomList(romList)
{
}

void Gui::run()
{
    // Searching for a suitable video mode
    spdlog::debug("Searching for a suitable video mode");
    auto availableVideoModes = sf::VideoMode::getFullscreenModes();
    if (availableVideoModes.empty())
    {
        spdlog::error("No suitable screen resolution found");
        return;
    }
    auto videoMode = availableVideoModes[0];

    spdlog::info("Creating application window with mode: {}x{}x{}", videoMode.width, videoMode.height,
                 videoMode.bitsPerPixel);

    sf::RenderWindow window(availableVideoModes[0], std::string(projectName), sf::Style::Fullscreen);
    window.setMouseCursorVisible(false);
    window.setFramerateLimit(MAX_FRAME_RATE);
    sf::View view(sf::FloatRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT));
    window.setView(view);

    // Drawing bottom info
    const float PROGRAM_INFO_X = view.getSize().x / 96.0F;
    const float PROGRAM_INFO_Y = view.getSize().y / 1.08F;
    ProgramInfo programInfo;
    programInfo.setPosition(PROGRAM_INFO_X, PROGRAM_INFO_Y);

    // Drawing rom menu
    const float ROM_MENU_X = view.getSize().x / 9.5F;
    const float ROM_MENU_Y = view.getSize().y / 6.0F;
    RomMenu romMenu(mRomList);
    romMenu.setPosition(ROM_MENU_X, ROM_MENU_Y);

    // Drawing No Rom Found text
    const float NO_ROM_FOUND_X = view.getSize().x / 2;
    const float NO_ROM_FOUND_Y = view.getSize().y / 2;
    TextNode noRomFound;
    noRomFound.element().setFont(FontManager::get().getResource("fonts/inter.ttf"));
    noRomFound.element().setCharacterSize(32);
    noRomFound.element().setString("No rom found");
    noRomFound.element().setFillColor(sf::Color::Red);
    noRomFound.element().setOrigin(noRomFound.element().getGlobalBounds().width / 2,
                                   noRomFound.element().getGlobalBounds().height / 2);
    noRomFound.element().setPosition(NO_ROM_FOUND_X, NO_ROM_FOUND_Y);

    // Creating sounds
    sf::Sound selectionSound;
    selectionSound.setBuffer(SoundManager::get().getResource("audio/move.wav"));

    sf::Sound launchSound;
    launchSound.setBuffer(SoundManager::get().getResource("audio/launch.wav"));

    // Connecting signals
    InputManager inputmanager;
    inputmanager.closeWindow.connect([&window]() { window.close(); });
    inputmanager.goDown.connect([&romMenu, &selectionSound]() {
        if (romMenu.selectionDown())
        {
            selectionSound.play();
        }
    });

    inputmanager.goUp.connect([&romMenu, &selectionSound]() {
        if (romMenu.selectionUp())
        {
            selectionSound.play();
        }
    });

    inputmanager.select.connect([this, &inputmanager, &romMenu, &launchSound]() {
        if (!mRomList.empty())
        {
            launchSound.play();
            Emulator emulator;
            if (auto err = emulator.run(*(romMenu.selectedRom()), inputmanager.getDevices()); err)
            {
                spdlog::error("Error launching rom: {}", magic_enum::enum_name(*err));
            }
        }
    });

    while (window.isOpen())
    {
        inputmanager.manage(window);

        window.clear();
        window.draw(programInfo);
        mRomList.empty() ? window.draw(noRomFound) : window.draw(romMenu);
        window.display();
    }
}
