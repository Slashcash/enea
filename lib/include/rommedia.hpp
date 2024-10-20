#ifndef ROMMEDIA_HPP
#define ROMMEDIA_HPP

#include <nlohmann/json.hpp>

/**
 * @brief This struct represents a set of media associated to a rom. Roms are generally accompanied
 * by screenshots, videos, sfx that are used in frontend to get a fancier presentation.
 */
struct RomMedia
{
    /**
     * @brief The label for the screenshot path in the json file
     *
     */
    static constexpr std::string_view SCREENSHOT_JSON_FIELD = "screenshot";

    /**
     * @brief Get the path where to locate a rom screenshot. This game
     * screenshot can be used anywhere to enhance the presentation in the gui
     *
     */
    std::optional<std::filesystem::path> screenshot;
};

/**
 * @brief nlohmann::json helper function. It is used by the nlohmann::json
 * library in order to automatically convert the RomMedia struct to a json.
 *
 * @param json the output json
 * @param info the input RomMedia struct
 */
void to_json(nlohmann::json& json, const RomMedia& media);
/**
 * @brief nlohmann::json helper function. It is used by the nlohmann::json
 * library in order to automatically obtain a RomMedia from a json.
 *
 * @param json the input json
 * @param info the output RomMedia struct
 */
void from_json(const nlohmann::json& json, RomMedia& media);

#endif // ROMMEDIA_HPP
