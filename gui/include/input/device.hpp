#ifndef Device_HPP
#define Device_HPP

#include <string>

#include <SFML/Window/Event.hpp>
#include <spdlog/spdlog.h>

#include "database.hpp"
#include "emulator.hpp"
#include "input/identification.hpp"
#include "input/mapping.hpp"

namespace Input {

class Device
{
 public:
    class Exception : public enea::Exception
    {
        using enea::Exception::Exception;
    };

    Device() = delete;
    explicit Device(const Input::Identification& identification, const unsigned int id);
    explicit Device(const Input::Identification& identification, const unsigned int id, const Mapping& mapping);

    [[nodiscard]] bool checkEvent(const Input::Frontend::Command& command, const sf::Event& event) const;
    [[nodiscard]] std::string getEmulatorInputString(const Emulator::Command& command) const;
    [[nodiscard]] unsigned int getId() const;
    [[nodiscard]] Input::Identification getIdentification() const;

    auto operator<=>(const Device& other) const
    {
        if (this->mIdentification.type == other.mIdentification.type)
        {
            return this->mId <=> other.mId;
        }

        return this->mIdentification.type <=> other.mIdentification.type;
    }

    bool operator==(const Device& other) const;

 private:
    using FrontendFunction = std::function<bool(const sf::Event& event)>;
    static const Input::Mapping DEFAULT_KEYBOARD_MAPPING;
    static const Input::Mapping DEFAULT_JOYSTICK_MAPPING;
    static const Input::Mapping DEFAULT_MAPPING;
    static const std::unordered_map<std::string, unsigned int> DEFAULT_BUTTON_TO_FRONTEND_JOYSTICK_MAPPING;

    unsigned int mId;
    Input::Identification mIdentification;
    Input::Mapping mMapping{DEFAULT_MAPPING};

    [[nodiscard]] std::optional<Input::Emulator::Button> getEmulatorButton(const Emulator::Command& command) const;
    [[nodiscard]] std::optional<Input::Frontend::Button> getFrontendButton(const Frontend::Command& command) const;
};

static constexpr char dbPath[] = "inputdb/inputdb.json";
using Database = DB<Input::Identification, Input::Mapping, dbPath>;

} // namespace Input

#endif // Device_HPP
