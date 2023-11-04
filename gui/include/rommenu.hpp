#ifndef ROMMENU_HPP
#define ROMMENU_HPP

#include <mutex>
#include <optional>
#include <rocket.hpp>
#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "rom.hpp"

class RomSource;
namespace sf {
class RenderTarget;
}

class RomMenu : public sf::Drawable, public sf::Transformable
{
 private:
    mutable std::mutex mMutex;
    unsigned int mSelected = 0;
    std::vector<Rom> mRomList; // ACCESS TO THIS SHOULD BE THREAD SAFE
    rocket::scoped_connection mAddedConnection;
    rocket::scoped_connection mDeletedConnection;

    void romAdded(const Rom& rom);
    void romDeleted(const Rom& rom);
    [[nodiscard]] bool setSelected(const unsigned int selected);

 public:
    RomMenu() = delete;
    explicit RomMenu(const RomSource& romSource);

    [[nodiscard]] std::optional<Rom> selectedRom() const;
    [[nodiscard]] inline bool selectedUp()
    {
        return setSelected(mSelected - 1);
    }
    [[nodiscard]] inline bool selectedDown()
    {
        return setSelected(mSelected + 1);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    mutable rocket::signal<void(const std::optional<Rom>& rom)> selectionChanged;
};

#endif // ROMMENU_HPP