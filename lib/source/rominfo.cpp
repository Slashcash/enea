#include "rominfo.hpp"

void to_json(nlohmann::json& json, const RomInfo& info)
{
    json.clear();

    // Setting rom title
    if (auto title = info.title; title)
    {
        json[RomInfo::TITLE_JSON_FIELD] = *title;
    }

    // Setting rom year
    if (auto year = info.year; year)
    {
        json[RomInfo::YEAR_JSON_FIELD] = *year;
    }

    // Setting rom manufacturer
    if (auto manufacturer = info.manufacturer; manufacturer)
    {
        json[RomInfo::MANUFACTURER_JSON_FIELD] = *manufacturer;
    }

    // Setting if it is bios
    if (auto isBios = info.isBios; isBios)
    {
        json[RomInfo::ISBIOS_JSON_FIELD] = *isBios;
    }
}

void from_json(const nlohmann::json& json, RomInfo& info)
{
    // Finding rom title
    try
    {
        info.title = json.at(RomInfo::TITLE_JSON_FIELD).get<std::string>();
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        // Since title is optional we just reset the value to empty if it's not found
        info.title.reset();
    }

    // Finding rom year
    try
    {
        info.year = json.at(RomInfo::YEAR_JSON_FIELD).get<std::string>();
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        // Since year is optional we just reset the value to empty if it's not found
        info.year.reset();
    }

    // Finding rom manufacturer
    try
    {
        info.manufacturer = json.at(RomInfo::MANUFACTURER_JSON_FIELD).get<std::string>();
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        // Since manufacturer is optional we just reset the value to empty if it's not found
        info.manufacturer.reset();
    }

    // Finding if rom is a bios
    try
    {
        info.isBios = json.at(RomInfo::ISBIOS_JSON_FIELD).get<bool>();
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        // Since manufacturer is optional we just reset the value to empty if it's not found
        info.isBios.reset();
    }
}
