#ifndef Device_HPP
#define Device_HPP

#include <string>

#include <SFML/Window/Event.hpp>

#include "emulator.hpp"
#include "inputmapping.hpp"

class Emulator;

namespace Input {

namespace Frontend {
enum class Command
{
    Up,
    Down,
    Enter,
    Esc
};
}

class Device
{
 public:
    enum class Type
    {
        Joystick = 0,
        Keyboard = 1
    };

    class Excep : public Exception
    {
        using Exception::Exception;
    };

    Device() = delete;
    explicit Device(const Type& type, const unsigned int id, const std::string& name);

    [[nodiscard]] Type getType() const;
    [[nodiscard]] bool checkEvent(const Input::Frontend::Command& command, const sf::Event& event) const;
    [[nodiscard]] std::string getEmulatorInputString(const Emulator::Command& command) const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] unsigned int getId() const;

    auto operator<=>(const Device& other) const
    {
        if (this->mType == other.mType)
        {
            return this->mId <=> other.mId;
        }

        return this->mType <=> other.mType;
    }

    bool operator==(const Device& other) const;

 private:
    using FrontendFunction = std::function<bool(const sf::Event& event)>;
    static const Input::Mapping DEFAULT_KEYBOARD_MAPPING;
    static const Input::Mapping DEFAULT_JOYSTICK_MAPPING;
    static const Input::Mapping DEFAULT_MAPPING;
    static const std::unordered_map<Input::Frontend::Command, Input::Emulator::Command>
        DEFAULT_FRONTEND_TO_EMULATOR_MAPPING;
    static const std::unordered_map<std::string, sf::Keyboard::Key> DEFAULT_BUTTON_TO_FRONTEND_KEYBOARD_MAPPING;
    static const std::unordered_map<std::string, unsigned int> DEFAULT_BUTTON_TO_FRONTEND_JOYSTICK_MAPPING;

    Type mType;
    unsigned int mId;
    std::string mName;
    Input::Mapping mMapping{DEFAULT_MAPPING};

    [[nodiscard]] std::optional<Input::Button> getEmulatorButton(const Emulator::Command& command) const;
    [[nodiscard]] std::optional<Input::Button> getFrontendButton(const Frontend::Command& command) const;
    [[nodiscard]] FrontendFunction getFrontendCheckFunction(const Frontend::Command& command,
                                                            const sf::Event& event) const;
};
} // namespace Input

#endif // Device_HPP
