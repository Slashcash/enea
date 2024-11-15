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
#include "input/button.hpp"
#include "utils.hpp"

namespace Input {

class Mapping
{
 private:
    /**
     * @brief This map contains the mapping between an advancemame command
     * and its associated button
     */
    std::unordered_map<Input::Emulator::Command, Emulator::Button> mEmulatorMapping;
    /**
     * @brief This map contains the association between a frontend command
     * and its associated button
     *
     */
    std::unordered_map<Input::Frontend::Command, Frontend::Button> mFrontendMapping;

 public:
    friend struct nlohmann::adl_serializer<Mapping>;

    /**
     * @brief The label for the emulator mapping in the json file
     *
     */
    static constexpr std::string_view EMULATORMAPPING_JSON_FIELD = "emulator";
    /**
     * @brief The label for the frontend mapping in the json file
     *
     */
    static constexpr std::string_view FRONTENDMAPPING_JSON_FIELD = "frontend";

    Mapping() = delete;
    /**
     * @brief Construct a new Mapping object
     *
     * @param emulatorMap The association between the emulator commands and their associated buttons
     * @param frontendMap The association between a frontend command and an associated button
     */
    explicit Mapping(const std::unordered_map<Input::Emulator::Command, Emulator::Button>& emulatorMap,
                     const std::unordered_map<Input::Frontend::Command, Frontend::Button>& frontendMap);

    /**
     * @brief Get the Emulator::Button associated to a specific emulator command, if any
     *
     * @param command the emulator command
     * @return std::optional<Emulator::Button> the associated button mapping, if any
     */
    [[nodiscard]] std::optional<Emulator::Button> getButton(Input::Emulator::Command command) const;
    /**
     * @brief Get the Frontend::Button associated to a specific emulator command, if any
     *
     * @param command the frontend command
     * @return std::optional<Frontend::Button> the associated button mapping, if any
     */
    [[nodiscard]] std::optional<Frontend::Button> getButton(Input::Frontend::Command command) const;

    [[nodiscard]] inline std::string toString() const
    {
        // This is just temporary, we should really have a more complete toString implementation
        return "Joystick Mapping";
    }
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

        for (const auto& [cmd, button] : mapping.mEmulatorMapping)
        {
            json[Input::Mapping::EMULATORMAPPING_JSON_FIELD][magic_enum::enum_name(cmd)] = button;
        }

        for (const auto& [cmd, button] : mapping.mFrontendMapping)
        {
            json[Input::Mapping::FRONTENDMAPPING_JSON_FIELD][magic_enum::enum_name(cmd)] = button;
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
        // EMULATOR MAPPINGS
        std::unordered_map<Input::Emulator::Command, Input::Emulator::Button> emulatorMapping;

        if (!json.contains(Input::Mapping::EMULATORMAPPING_JSON_FIELD))
        {
            throw enea::Exception("Error creating mapping from json, emulator mappings not found");
        }

        auto mappings = json[Input::Mapping::EMULATORMAPPING_JSON_FIELD];

        for (auto entry = mappings.begin(); entry != mappings.end(); entry++)
        {
            try
            {
                auto cmd = magic_enum::enum_cast<Input::Emulator::Command>(entry.key());
                if (!cmd)
                {
                    throw(enea::Exception("Invalid mapping json"));
                }

                emulatorMapping.try_emplace(*cmd, entry.value().get<Input::Emulator::Button>());
            }
            catch (...)
            {
                throw(enea::Exception("Invalid mapping json"));
            }
        }

        // FRONTEND MAPPINGS
        std::unordered_map<Input::Frontend::Command, Input::Frontend::Button> frontendMapping;

        if (!json.contains(Input::Mapping::FRONTENDMAPPING_JSON_FIELD))
        {
            throw enea::Exception("Error creating mapping from json, frontend mappings not found");
        }

        mappings = json[Input::Mapping::FRONTENDMAPPING_JSON_FIELD];

        for (auto entry = mappings.begin(); entry != mappings.end(); entry++)
        {
            try
            {
                auto cmd = magic_enum::enum_cast<Input::Frontend::Command>(entry.key());
                if (!cmd)
                {
                    throw(enea::Exception("Invalid mapping json"));
                }

                frontendMapping.try_emplace(*cmd, entry.value().get<Input::Frontend::Button>());
            }
            catch (...)
            {
                throw(enea::Exception("Invalid mapping json"));
            }
        }

        return Input::Mapping(emulatorMapping, frontendMapping);
    }
};
} // namespace nlohmann

template <> struct fmt::formatter<Input::Mapping> : fmt::formatter<string_view>
{
    auto format(const Input::Mapping& mapping, fmt::format_context& ctx) const -> fmt::format_context::iterator
    {
        return fmt::formatter<string_view>::format(mapping.toString(), ctx);
    }
};

#endif
