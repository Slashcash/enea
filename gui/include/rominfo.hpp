#ifndef ROMINFO_HPP
#define ROMINFO_HPP

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
    std::optional<Rom> mRom;
    rocket::scoped_connection mSelectionConnection;
    sf::Font mFont;

    inline void selectionChanged(const std::optional<Rom>& rom)
    {
        mRom = rom;
    }

 public:
    RomInfo() = delete;
    explicit RomInfo(const RomMenu& romMenu);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // ROMINFO_HPP