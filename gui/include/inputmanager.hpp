#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <functional>
#include <list>

#include <SFML/Window/Event.hpp>
#include <rocket.hpp>

#include "inputdevice.hpp"

namespace sf {
class Event;
}
namespace sf {
class RenderWindow;
}

class InputManager
{
 private:
    // This is pretty much a limitation of SFML, it is anyway a sufficiently high number for us
    static constexpr unsigned int MAX_JOYSTICK = sf::Joystick::Count;

    using InputFunction = std::function<bool(const sf::Event& event)>;
    using Signal = rocket::signal<void()>;

    struct Reaction
    {
        InputFunction t;
        Signal* r;
    };

    std::list<Reaction> mReactions;
    std::vector<Input::Device> mAvailableInputs;

    [[nodiscard]] virtual std::list<sf::Event> events(sf::RenderWindow& window) const;
    void addDevice(const Input::Device& device);
    void removeDevice(const Input::Device& device);

 public:
    InputManager();
    InputManager(const InputManager& manager) = delete;
    InputManager(InputManager&& manager) = delete;

    void manage(sf::RenderWindow& window);
    std::vector<Input::Device> getDevices() const;

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
