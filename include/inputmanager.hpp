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

    InputManager(InputManager& manager) = delete;

    [[nodiscard]] virtual std::list<sf::Event> events(sf::RenderWindow& window) const;

    void operator=(const InputManager& manager) = delete;

 public:
    [[nodiscard]] inline static InputManager& get()
    {
        static InputManager manager;
        return manager;
    }
    void manage(sf::RenderWindow& window) const;

    Signal closeWindow;
    Signal goUp;
    Signal goDown;
    Signal select;

 protected:
    InputManager();

    ~InputManager() = default;
};

#endif // INPUTMANAGER_HPP