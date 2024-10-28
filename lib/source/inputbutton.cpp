#include "inputbutton.hpp"

#include <fmt/format.h>
#include <magic_enum.hpp>

#include "exception.hpp"
#include "utils.hpp"

using namespace Input;

void Input::to_json(nlohmann::json& json, const Button& button)
{
    json.clear();

    // Setting button type
    json[Button::TYPE_JSON_FIELD] = magic_enum::enum_name(button.type);

    // Setting button definition
    json[Button::DEFINITION_JSON_FIELD] = button.definition;
}

void Input::from_json(const nlohmann::json& json, Button& button)
{
    // Finding button type
    auto jsonType = utils::getValueFromJson<std::string>(json, Button::TYPE_JSON_FIELD);
    if (!jsonType)
    {
        throw Exception("Invalid input json: missing button type");
    }

    auto optType = magic_enum::enum_cast<Button::Type>(*jsonType);
    if (!optType)
    {
        throw Exception(fmt::format("Invalid input type in json: {}", *jsonType));
    }

    button.type = *optType;

    // Finding button definition
    auto jsonDefinition = utils::getValueFromJson<std::string>(json, Button::DEFINITION_JSON_FIELD);
    if (!jsonDefinition)
    {
        throw Exception("Invalid input json: missing button definition");
    }

    button.definition = *jsonDefinition;
}
