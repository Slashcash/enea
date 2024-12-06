#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <functional>
#include <list>

#include <SFML/Window/Event.hpp>
#include <rocket.hpp>

#include "database.hpp"
#include "input/device.hpp"
#include "input/identification.hpp"

namespace sf {
class Event;
}
namespace sf {
class RenderWindow;
}

namespace Input {
class Manager
{
 private:
    // This is pretty much a limitation of SFML, it is anyway a sufficiently high number for us
    static constexpr unsigned int MAX_JOYSTICK = sf::Joystick::Count;
    static constexpr unsigned int MAX_PLAYERS = 4;

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

    [[nodiscard]] static unsigned int getNumberDevices(const std::vector<Input::Device>& availableInputs);
    [[nodiscard]] static std::string inputString(const Input::Device& device, const Input::Emulator::Command& command);
    [[nodiscard]] static unsigned int getNumberOfPlayers(const std::vector<Input::Device>& availableInputs);
    [[nodiscard]] static std::optional<Input::Device>
    mapDeviceToPlayerNumber(const std::vector<Input::Device>& availableInput, const unsigned int playerNum);
    [[nodiscard]] static unsigned int mapInputToPlayerNumber(const Input::Emulator::Command& command);

 public:
    class Exception : public enea::Exception
    {
        using enea::Exception::Exception;
    };

    Manager();
    Manager(const Manager& manager) = delete;
    Manager(Manager&& manager) = delete;

    void manage(sf::RenderWindow& window);
    std::vector<Input::Device> getDevices() const;

    Manager& operator=(const Manager& manager) = delete;
    Manager& operator=(Manager&& manager) = delete;

    // NOLINTBEGIN (these are really useful as public members)
    Signal closeWindow;
    Signal goUp;
    Signal goDown;
    Signal select;
    // NOLINTEND

    virtual ~Manager() = default;

    [[nodiscard]] std::string controlString() const;
};
} // namespace Input

#endif // INPUTMANAGER_HPP
