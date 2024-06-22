#include "rommedia.hpp"

void to_json(nlohmann::json& json, const RomMedia& media)
{
    json.clear();

    // Setting screenshot
    if (const auto& screenshot = media.screenshot; screenshot)
    {
        json[RomMedia::SCREENSHOT_JSON_FIELD] = *screenshot;
    }
}

void from_json(const nlohmann::json& json, RomMedia& media)
{
    // Finding screenshot
    try
    {
        media.screenshot = json.at(RomMedia::SCREENSHOT_JSON_FIELD).get<std::string>();
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        // Since screenshot is optional we just reset the value to empty if it's not found
        media.screenshot.reset();
    }
}
