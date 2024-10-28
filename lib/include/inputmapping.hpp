#ifndef MAPPING_HPP
#define MAPPING_HPP

#include <unordered_map>

#include <SFML/Window/Joystick.hpp>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/json.hpp>

#include "emulator.hpp"
#include "exception.hpp"
#include "inputbutton.hpp"
#include "utils.hpp"

namespace Input {

class Mapping
{
 private:
    /**
     * @brief This map contains the mapping between an advancemame command
     * and its associated button
     */
    std::unordered_map<Input::Emulator::Command, Button> mButtonMapping;
    /**
     * @brief This map contains the association between an advancemame axis number
     * and its equivalent in the sfml world
     *
     */
    std::unordered_map<unsigned int, sf::Joystick::Axis> mAxisMapping;

 public:
    friend struct nlohmann::adl_serializer<Mapping>;

    /**
     * @brief The label for the button mapping in the json file
     *
     */
    static constexpr std::string_view MAPPING_JSON_FIELD = "mapping";
    /**
     * @brief The label for the axis in the json file
     *
     */
    static constexpr std::string_view AXIS_JSON_FIELD = "axis";

    Mapping() = delete;
    /**
     * @brief Construct a new Mapping object
     *
     * @param buttonMap The association between the emulator commands and their associated buttons
     */
    explicit Mapping(const std::unordered_map<Input::Emulator::Command, Button>& buttonMap);
    /**
     * @brief Construct a new Mapping object
     *
     * @param buttonMap The association between the emulator commands and their associated buttons
     * @param axisMap The association between an advancemame axis and its sfml counterpart
     */
    explicit Mapping(const std::unordered_map<Input::Emulator::Command, Button>& buttonMap,
                     const std::unordered_map<unsigned int, sf::Joystick::Axis>& axisMap);

    /**
     * @brief Get the Button associated to a specific emulator command, if any
     *
     * @param command the emulator command
     * @return std::optional<Button> the associated button mapping, if any
     */
    [[nodiscard]] std::optional<Button> getButton(Input::Emulator::Command command) const;
};
} // namespace Input

namespace nlohmann {
template <> struct adl_serializer<Input::Mapping>
{
    /**
     * @brief nlohmann::json helper function. It is used by the nlohmann::json
     * library in order to automatically convert the Mapping class to a json.
     *
     * @param json the output json
     * @param info the input Mapping class
     */
    static void to_json(json& json, Input::Mapping const& mapping)
    {
        json.clear();

        for (const auto& [cmd, button] : mapping.mButtonMapping)
        {
            json[Input::Mapping::MAPPING_JSON_FIELD][magic_enum::enum_name(cmd)] = button;
        }

        for (const auto& [axismame, axissfml] : mapping.mAxisMapping)
        {
            json[Input::Mapping::AXIS_JSON_FIELD][std::to_string(axismame)] = magic_enum::enum_name(axissfml);
        }
    }
    /**
     * @brief nlohmann::json helper function. It is used by the nlohmann::json
     * library in order to automatically obtain a mapping from a json.
     *
     * @param json the input json
     * @return the output Mapping class
     */
    static Input::Mapping from_json(const json& json)
    {
        // MAPPINGS
        std::unordered_map<Input::Emulator::Command, Input::Button> mapping;

        if (!json.contains(Input::Mapping::MAPPING_JSON_FIELD))
        {
            throw Exception("Error creating mapping from json, mappings not found");
        }

        auto mappings = json[Input::Mapping::MAPPING_JSON_FIELD];

        for (auto entry = mappings.begin(); entry != mappings.end(); entry++)
        {
            try
            {
                auto cmd = magic_enum::enum_cast<Input::Emulator::Command>(entry.key());
                if (!cmd)
                {
                    throw(Exception("Invalid mapping json"));
                }

                mapping.try_emplace(*cmd, entry.value().get<Input::Button>());
            }
            catch (...)
            {
                throw(Exception("Invalid mapping json"));
            }
        }

        // AXIS
        std::unordered_map<unsigned int, sf::Joystick::Axis> ax;

        if (json.contains(Input::Mapping::AXIS_JSON_FIELD))
        {
            auto axis = json[Input::Mapping::AXIS_JSON_FIELD];

            for (auto entry = axis.begin(); entry != axis.end(); entry++)
            {
                auto axe = magic_enum::enum_cast<sf::Joystick::Axis>(entry.value().get<std::string>());
                if (!axe)
                {
                    throw Exception("Invalid mapping json: invalid axis");
                }

                ax.try_emplace(std::stoul(entry.key()), *axe);
            }
        }

        return Input::Mapping(mapping, ax);
    }
};
} // namespace nlohmann

#endif
