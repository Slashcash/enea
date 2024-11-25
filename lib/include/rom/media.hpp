#ifndef ROMMEDIA_HPP
#define ROMMEDIA_HPP

#include <nlohmann/json.hpp>

#include "utils.hpp"

namespace Rom {

struct Media
{
    static constexpr std::string_view SCREENSHOT_JSON_FIELD = "screenshot";
    std::optional<std::filesystem::path> screenshot;
};

inline void to_json(nlohmann::json& json, const Rom::Media& media)
{
    json.clear();

    utils::addOptionalToJson(json, Rom::Media::SCREENSHOT_JSON_FIELD, media.screenshot);
}

inline void from_json(const nlohmann::json& json, Rom::Media& media)
{
    media.screenshot = utils::getOptionalValueFromJson<std::filesystem::path>(json, Rom::Media::SCREENSHOT_JSON_FIELD);
}
} // namespace Rom

#endif // ROMMEDIA_HPP
