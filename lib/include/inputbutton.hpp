#ifndef INPUTBUTTON_HPP
#define INPUTBUTTON_HPP

#include <nlohmann/json.hpp>

namespace Input {
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
void to_json(nlohmann::json& json, const Input::Button& button);
/**
 * @brief nlohmann::json helper function. It is used by the nlohmann::json
 * library in order to automatically obtain a Button from a json.
 *
 * @param json the input json
 * @param info the output Input struct
 */
void from_json(const nlohmann::json& json, Input::Button& button);

} // namespace Input

#endif // INPUTBUTTON_HPP
