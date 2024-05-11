#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <functional>
#include <list>

#include <rocket.hpp>

namespace sf {
class Event;
}
namespace sf {
class RenderWindow;
}

class InputManager
{
 private:
    using InputFunction = std::function<bool(const sf::Event& event)>;
    using Signal = rocket::signal<void()>;

    struct Reaction
    {
        InputFunction t;
        Signal* r;
    };

    std::list<Reaction> mReactions;
    [[nodiscard]] virtual std::list<sf::Event> events(sf::RenderWindow& window) const;

 public:
    InputManager();
    InputManager(const InputManager& manager) = delete;
    InputManager(InputManager&& manager) = delete;

    void manage(sf::RenderWindow& window) const;

    InputManager& operator=(const InputManager& manager) = delete;
    InputManager& operator=(InputManager&& manager) = delete;

    // NOLINTBEGIN (these are really useful as public members)
    Signal closeWindow;
    Signal goUp;
    Signal goDown;
    Signal select;
    // NOLINTEND

    virtual ~InputManager() = default;
};

#endif // INPUTMANAGER_HPP
