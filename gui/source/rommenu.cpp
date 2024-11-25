#include "rommenu.hpp"

#include "externalresourcemanager.hpp"

#include <spdlog/spdlog.h>

RomMenu::RomMenu(const std::vector<Rom::Game>& roms)
{
    for (const auto& rom : roms)
    {
        mRoms.push_back(rom);
    }

    std::ranges::sort(mRoms, [](const Rom::Game& first, const Rom::Game& second) {
        std::string firstTitle = first.info().title;
        std::string secondTitle = second.info().title;

        return firstTitle < secondTitle;
    });

    reorganize();
}

std::string RomMenu::shortenedRomName(const Rom::Game& rom)
{
    auto result = romName(rom);
    if (result.size() > MAX_CHAR_SIZE_NAME)
    {
        result = result.substr(0, MAX_CHAR_SIZE_NAME) + "...";
    }

    return result;
}

std::string RomMenu::romName(const Rom::Game& rom)
{
    std::string result;

    result = rom.info().title;
    result = result.substr(0, result.find_first_of('('));

    return result;
}

bool RomMenu::setSelected(const unsigned int selected)
{
    if (selected < mRoms.size())
    {
        mSelected = selected;
        reorganize();
        return true;
    }

    return false;
}

void RomMenu::reorganize()
{
    // No need to do anything if there is no rom to draw
    if (!mRoms.empty())
    {
        deleteChildren();
        const unsigned long start = (mSelected / ROWS) * ROWS;
        // This cast shouldn't be needed but we get compilation errors in armv7hf (?)
        const unsigned long stop = std::min(static_cast<std::size_t>((mSelected / ROWS) * ROWS + ROWS), mRoms.size());

        std::shared_ptr<TextNode> prev;

        for (unsigned long i = start; i < stop; i++)
        {
            auto rom = mRoms[i];
            auto row = std::make_shared<TextNode>();
            row->element().setString(shortenedRomName(rom));
            row->element().setFont(mFont);
            row->element().setCharacterSize(LIST_CHAR_SIZE);
            i == mSelected ? row->element().setFillColor(sf::Color::White)
                           : row->element().setFillColor(sf::Color::Red);

            // If it is the first element we just attach it with no offset
            if (!prev)
            {
                addChild(row);
            }
            else
            {
                row->setPosition(0, prev->element().getGlobalBounds().height + TEXT_SPACING);
                prev->addChild(row);
            }

            prev = row;
        }

        // Drawing rom info
        auto rom = mRoms[mSelected];
        auto nameText = std::make_shared<TextNode>();
        nameText->element().setString(romName(rom));
        nameText->element().setFont(mFont);
        nameText->element().setCharacterSize(INFO_CHAR_SIZE);
        nameText->element().setFillColor(sf::Color::White);
        // NOLINTNEXTLINE
        nameText->setOrigin(nameText->element().getGlobalBounds().width / 2.0F, 0);
        nameText->setPosition(SCREENSHOT_X_OFFSET, SCREENSHOT_Y_OFFSET);
        addChild(nameText);

        auto year = rom.info().year ? *(rom.info().year) : "Unknown Year";
        auto manufacturer = rom.info().manufacturer ? *(rom.info().manufacturer) : "Unknown Manufacturer";
        auto infoText = std::make_shared<TextNode>();
        infoText->element().setString(fmt::format("{}, {}", year, manufacturer));
        infoText->element().setFont(mFont);
        infoText->element().setCharacterSize(INFO_CHAR_SIZE);
        infoText->element().setFillColor(sf::Color::White);
        // NOLINTNEXTLINE
        infoText->setOrigin(infoText->element().getGlobalBounds().width / 2.0F, 0);
        // NOLINTNEXTLINE
        infoText->setPosition(nameText->element().getGlobalBounds().width / 2.0F,
                              nameText->element().getGlobalBounds().height + TEXT_SPACING);
        nameText->addChild(infoText);

        // Drawing rom screenshot
        try
        {
            if (auto media = rom.media(); media && media->screenshot)
            {
                auto screenshotSprite = std::make_shared<SpriteNode>();
                screenshotSprite->element().setTexture(ScreenShotManager::get().getResource(*(media->screenshot)));
                auto screenshotWidth = screenshotSprite->element().getGlobalBounds().width;
                auto screenshotHeight = screenshotSprite->element().getGlobalBounds().height;
                // NOLINTNEXTLINE
                screenshotSprite->setOrigin(screenshotWidth / 2.0F, 0);
                // NOLINTNEXTLINE
                screenshotSprite->setPosition(infoText->element().getGlobalBounds().width / 2.0F,
                                              infoText->element().getGlobalBounds().height + SCREENSHOT_SPACING);
                screenshotSprite->scale(SCREENSHOT_WIDTH / screenshotWidth, SCREENSHOT_HEIGHT / screenshotHeight);
                infoText->addChild(screenshotSprite);
            }
        }
        catch (const ResourceManager<sf::Texture>::Exception& excep)
        {
            // Don't do anything is no screenshot is available, it's fine
        }
    }
}

bool RomMenu::selectionDown()
{
    return setSelected(mSelected + 1);
}

bool RomMenu::selectionUp()
{
    return setSelected(mSelected - 1);
}

std::optional<Rom::Game> RomMenu::selectedRom() const
{
    return mRoms[mSelected];
}
