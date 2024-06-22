#ifndef ROMMEDIA_HPP
#define ROMMEDIA_HPP

#include <nlohmann/json.hpp>

struct RomMedia
{
    static constexpr std::string_view SCREENSHOT_JSON_FIELD = "screenshot";

    std::optional<std::filesystem::path> screenshot;
};

void to_json(nlohmann::json& json, const RomMedia& media);
void from_json(const nlohmann::json& json, RomMedia& media);

#endif // ROMMEDIA_HPP
