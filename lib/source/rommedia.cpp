#include "rommedia.hpp"

#include "utils.hpp"

void to_json(nlohmann::json& json, const RomMedia& media)
{
    json.clear();

    // Setting screenshot
    utils::addOptionalToJson(json, RomMedia::SCREENSHOT_JSON_FIELD, media.screenshot);
}

void from_json(const nlohmann::json& json, RomMedia& media)
{
    // Finding screenshot
    media.screenshot = utils::getValueFromJson<std::filesystem::path>(json, RomMedia::SCREENSHOT_JSON_FIELD);
}
