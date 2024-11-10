#include "input/button.hpp"

#include <fmt/format.h>
#include <magic_enum.hpp>

#include "exception.hpp"
#include "utils.hpp"

void Input::Emulator::to_json(nlohmann::json& json, const Input::Emulator::Button& button)
{
    json.clear();

    // Setting button type
    json[Input::Emulator::Button::TYPE_JSON_FIELD] = magic_enum::enum_name(button.type);

    // Setting button definition
    json[Input::Emulator::Button::DEFINITION_JSON_FIELD] = button.definition;
}

void Input::Emulator::from_json(const nlohmann::json& json, Input::Emulator::Button& button)
{
    // Finding button type
    auto jsonType = utils::getOptionalValueFromJson<std::string>(json, Input::Emulator::Button::TYPE_JSON_FIELD);
    if (!jsonType)
    {
        throw enea::Exception("Invalid input json: missing button type");
    }

    auto optType = magic_enum::enum_cast<Input::Emulator::Button::Type>(*jsonType);
    if (!optType)
    {
        throw enea::Exception(fmt::format("Invalid input type in json: {}", *jsonType));
    }

    button.type = *optType;

    // Finding button definition
    auto jsonDefinition =
        utils::getOptionalValueFromJson<std::string>(json, Input::Emulator::Button::DEFINITION_JSON_FIELD);
    if (!jsonDefinition)
    {
        throw enea::Exception("Invalid input json: missing button definition");
    }

    button.definition = *jsonDefinition;
}
