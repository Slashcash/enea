#ifndef INPUTBUTTON_HPP
#define INPUTBUTTON_HPP

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <nlohmann/json.hpp>

#include "utils.hpp"

namespace Input::Emulator {
/**
 * @brief This struct represents a button as conceptualized by advMAME. A button
 * in advMAME can generally mapped as two strings: the kind of input (such as digital or analog, for example)
 * and its actual definition (which digital button or which analog range, for example)
 */
struct Button
{
    /**
     * @brief This enum represents all the possible
     * input types conceptualized by advMAME
     *
     */
    enum class Type
    {
        joystick_digital,
        joystick_button,
        keyboard
    };

    /**
     * @brief The label for the input type in the json file
     *
     */
    static constexpr std::string_view TYPE_JSON_FIELD = "type";
    /**
     * @brief The label for the input definition in the json file
     *
     */
    static constexpr std::string_view DEFINITION_JSON_FIELD = "definition";

    /**
     * @brief The input type. advMAME defines different input types such as digital, analogs and so on
     *
     */
    Type type;
    /**
     * @brief The input definition. advMAME uses a string in order to define an input: such as
     * which digital button is associated with that particolar input or which analog range is
     * considered to be an input
     *
     */
    std::string definition;

    [[nodiscard]] bool operator==(const Button& button) const = default;
};

/**
 * @brief nlohmann::json helper function. It is used by the nlohmann::json
 * library in order to automatically convert the Button struct to a json.
 *
 * @param json the output json
 * @param info the input Button struct
 */
void to_json(nlohmann::json& json, const Emulator::Button& button);
/**
 * @brief nlohmann::json helper function. It is used by the nlohmann::json
 * library in order to automatically obtain a Button from a json.
 *
 * @param json the input json
 * @param info the output Input struct
 */
void from_json(const nlohmann::json& json, Emulator::Button& button);

} // namespace Input::Emulator

namespace Input::Frontend {

enum class Command
{
    Up,
    Down,
    Enter,
    Esc
};

struct Axis
{
    static constexpr std::string_view AXIS_JSON_FIELD = "axis";
    static constexpr std::string_view RANGE_JSON_FIELD = "range";

    sf::Joystick::Axis joystickAxis;
    float range = 0;
};

inline void to_json(nlohmann::json& json, const Axis& axis)
{
    json.clear();
    json[Axis::AXIS_JSON_FIELD] = axis.joystickAxis;
    json[Axis::RANGE_JSON_FIELD] = axis.range;
}

inline void from_json(const nlohmann::json& json, Axis& axis)
{
    axis.joystickAxis = utils::getMandatoryValueFromJson<sf::Joystick::Axis>(json, Axis::AXIS_JSON_FIELD);
    axis.range = utils::getMandatoryValueFromJson<float>(json, Axis::RANGE_JSON_FIELD);
}

struct Button
{
    static constexpr std::string_view AXIS_JSON_FIELD = "axis";
    static constexpr std::string_view JOYSTICKBUTTON_JSON_FIELD = "joystickButton";
    static constexpr std::string_view KEYBOARDBUTTON_JSON_FIELD = "keyboardButton";

    std::optional<Axis> axis;
    std::optional<unsigned int> joystickButton;
    std::optional<sf::Keyboard::Key> keyboardButton;

    inline bool isPressed(const sf::Event& event) const
    {
        if (keyboardButton)
        {

            return event.type == sf::Event::KeyPressed && event.key.code == *keyboardButton;
        }
        else if (joystickButton)
        {
            return event.type == sf::Event::JoystickButtonPressed && event.joystickButton.button == *joystickButton;
        }
        else if (axis)
        {
            return event.type == sf::Event::JoystickMoved && event.joystickMove.axis == axis->joystickAxis &&
                   event.joystickMove.position == axis->range;
        }

        return false;
    }
};

inline void to_json(nlohmann::json& json, const Button& button)
{
    json.clear();
    if (button.axis)
    {
        json[Button::AXIS_JSON_FIELD] = *(button.axis);
    }
    else if (button.joystickButton)
    {
        json[Button::JOYSTICKBUTTON_JSON_FIELD] = *(button.joystickButton);
    }
    else if (button.keyboardButton)
    {
        json[Button::KEYBOARDBUTTON_JSON_FIELD] = *(button.keyboardButton);
    }
    else
    {
        throw enea::Exception("Invalid frontend button");
    }
}

inline void from_json(const nlohmann::json& json, Button& button)
{
    if (json.contains(Button::AXIS_JSON_FIELD))
    {
        button.axis = json[Button::AXIS_JSON_FIELD];
    }
    else if (json.contains(Button::JOYSTICKBUTTON_JSON_FIELD))
    {
        button.joystickButton = json[Button::JOYSTICKBUTTON_JSON_FIELD];
    }
    else if (json.contains(Button::KEYBOARDBUTTON_JSON_FIELD))
    {
        button.keyboardButton = json[Button::KEYBOARDBUTTON_JSON_FIELD];
    }
    else
    {
        throw enea::Exception("Invalid frontend button in json");
    }
}

} // namespace Input::Frontend

#endif // INPUTBUTTON_HPP
