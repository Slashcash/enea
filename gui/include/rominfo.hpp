#ifndef ROMINFO_HPP
#define ROMINFO_HPP

#include <mutex>
#include <optional>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <rocket.hpp>

#include "rom.hpp"

class RomMenu;
namespace sf {
class RenderTarget;
}

class RomInfo : public sf::Drawable, public sf::Transformable
{
 private:
    std::optional<Rom> mRom; // ACCESS TO THIS SHOULD BE THREAD SAFE
    rocket::scoped_connection mSelectionConnection;
    sf::Font mFont;
    mutable std::mutex mMutex;

    inline void selectionChanged(const std::optional<Rom>& rom)
    {
        std::scoped_lock lock(mMutex);
        mRom = rom;
    }

 public:
    RomInfo() = delete;
    explicit RomInfo(const RomMenu& romMenu);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // ROMINFO_HPP