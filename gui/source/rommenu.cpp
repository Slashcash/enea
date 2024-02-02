#include "rommenu.hpp"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <ext/alloc_traits.h>
#include <filesystem>
#include <functional>
#include <list>
#include <memory>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>

#include "resourcemanager.hpp"
#include "romsource.hpp"

RomMenu::RomMenu(const RomSource& romSource)
{
    auto romList = romSource.romList();
    for (const auto& rom : romList)
    {
        mRomList.push_back(rom);
    }

    std::ranges::sort(mRomList, [](const Rom& a, const Rom& b) {
        std::string aTitle;
        std::string bTitle;

        a.title().has_value() ? aTitle = *(a.title()) : aTitle = a.path().stem();
        b.title().has_value() ? bTitle = *(b.title()) : bTitle = b.path().stem();

        return aTitle < bTitle;
    });

    mAddedConnection = romSource.romAdded.connect([this](const Rom& rom) { romAdded(rom); });
    mDeletedConnection = romSource.romDeleted.connect([this](const Rom& rom) { romDeleted(rom); });
}

bool RomMenu::setSelected(const unsigned int selected)
{
    bool didSelectionChange = false;
    {
        std::scoped_lock lock(mMutex);

        if (selected < mRomList.size())
        {
            mSelected = selected;
            didSelectionChange = true;
        }
    }

    if (didSelectionChange)
    {
        auto newSelection = selectedRom();
        selectionChanged(newSelection);
    }

    return didSelectionChange;
}

void RomMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!mRomList.empty())
    {
        std::scoped_lock lock(mMutex);
        constexpr unsigned int menuRows = 15;
        constexpr unsigned int spacing = 16;

        const unsigned int start = (mSelected / menuRows) * menuRows;
        const unsigned int stop =
            std::min(static_cast<std::size_t>((mSelected / menuRows) * menuRows + menuRows), mRomList.size());

        std::vector<sf::Text> textVector;
        auto font = FontManager::get().getResource("fonts/inter.ttf");
        for (unsigned int i = start; i < stop; i++)
        {
            // Set text, font and size
            std::string romName;
            mRomList[i].title().has_value() ? romName = *(mRomList[i].title()) : romName = mRomList[i].path().stem();
            auto romNameShortened = romName.substr(0, romName.find_first_of('(')).substr(0, 25);
            sf::Text text(romNameShortened, font, 36);

            // Set color
            i == mSelected ? text.setFillColor(sf::Color::White) : text.setFillColor(sf::Color::Red);

            // Set position on y axis
            auto yOffset = textVector.empty() ? 0
                                              : textVector.back().getPosition().y +
                                                    textVector.back().getGlobalBounds().height + spacing;
            text.setPosition(0, yOffset);

            textVector.push_back(text);
        }

        states.transform *= this->getTransform();
        for (const auto& it : textVector)
        {
            target.draw(it, states);
        }
    }
}

std::optional<Rom> RomMenu::selectedRom() const
{
    std::scoped_lock lock(mMutex);
    return mRomList.empty() ? std::optional<Rom>() : mRomList[mSelected];
}

void RomMenu::romAdded(const Rom& rom)
{
    std::scoped_lock lock(mMutex);
    mRomList.push_back(rom);
    std::ranges::sort(mRomList, [](const Rom& a, const Rom& b) {
        std::string aTitle;
        std::string bTitle;

        a.title().has_value() ? aTitle = *(a.title()) : aTitle = a.path().stem();
        b.title().has_value() ? bTitle = *(b.title()) : bTitle = b.path().stem();

        return aTitle < bTitle;
    });

    if (mRomList.size() == 1)
    {
        selectionChanged(rom);
    }
}

void RomMenu::romDeleted(const Rom& rom)
{
    mRomList.erase(
        std::remove_if(mRomList.begin(), mRomList.end(), [&rom](const Rom& r) { return rom.path() == r.path(); }),
        mRomList.end());

    std::ranges::sort(mRomList, [](const Rom& a, const Rom& b) {
        std::string aTitle;
        std::string bTitle;

        a.title().has_value() ? aTitle = *(a.title()) : aTitle = a.path().stem();
        b.title().has_value() ? bTitle = *(b.title()) : bTitle = b.path().stem();

        return aTitle < bTitle;
    });

    if (mSelected >= mRomList.size() - 1)
    {
        [[maybe_unused]] auto error = setSelected(mRomList.size() - 1);
    }
}