#ifndef ROMMEDIA_HPP
#define ROMMEDIA_HPP

#include <nlohmann/json.hpp>

/**
 * @brief This struct represents a set of media associated to a rom. Roms are generally accompanied
 * by screenshots, videos, sfx that are used in frontend to get a fancier presentation.
 */
struct RomMedia
{
    static constexpr std::string_view SCREENSHOT_JSON_FIELD = "screenshot";

    std::optional<std::filesystem::path> screenshot;
};

void to_json(nlohmann::json& json, const RomMedia& media);
void from_json(const nlohmann::json& json, RomMedia& media);

#endif // ROMMEDIA_HPP
