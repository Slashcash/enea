#ifndef ROMMENU_HPP
#define ROMMENU_HPP

#include "emulator.hpp"
#include "internalresourcemanager.hpp"
#include "node.hpp"
#include "rom/game.hpp"

class RomMenu : public Node
{
 private:
    static constexpr unsigned long ROWS = 15;
    static constexpr float TEXT_SPACING = 16.0F;
    static constexpr float SCREENSHOT_SPACING = 40.0F;
    static constexpr unsigned int MAX_CHAR_SIZE_NAME = 38;
    static constexpr unsigned int LIST_CHAR_SIZE = 32;
    static constexpr unsigned int INFO_CHAR_SIZE = 22;
    static constexpr float SCREENSHOT_X_OFFSET = 1200.0F;
    static constexpr float SCREENSHOT_Y_OFFSET = 50.0F;
    static constexpr float SCREENSHOT_WIDTH = 750.0F;
    static constexpr float SCREENSHOT_HEIGHT = 428.0F;

    std::vector<Rom::Game> mRoms;
    unsigned long mSelected = 0;
    const sf::Font& mFont = FontManager::get().getResource("fonts/inter.ttf");

    void reorganize();
    [[nodiscard]] bool setSelected(unsigned int selected);
    [[nodiscard]] static std::string romName(const Rom::Game& rom);
    [[nodiscard]] static std::string shortenedRomName(const Rom::Game& rom);
    void inline drawEffective(sf::RenderTarget& target, sf::RenderStates states) const override {}

 public:
    RomMenu() = delete;
    explicit RomMenu(const std::vector<Rom::Game>& roms);

    [[nodiscard]] bool selectionDown();
    [[nodiscard]] bool selectionUp();
    [[nodiscard]] std::optional<Rom::Game> selectedRom() const;
};

#endif
