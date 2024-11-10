#include "rominfo.hpp"

#include "utils.hpp"

void to_json(nlohmann::json& json, const RomInfo& info)
{
    json.clear();

    // Setting rom title
    utils::addOptionalToJson(json, RomInfo::TITLE_JSON_FIELD, info.title);
    // Setting rom year
    utils::addOptionalToJson(json, RomInfo::YEAR_JSON_FIELD, info.year);
    // Setting rom manufacturer
    utils::addOptionalToJson(json, RomInfo::MANUFACTURER_JSON_FIELD, info.manufacturer);
    // Setting if it is bios
    utils::addOptionalToJson(json, RomInfo::ISBIOS_JSON_FIELD, info.isBios);
}

void from_json(const nlohmann::json& json, RomInfo& info)
{
    // Finding rom title
    info.title = utils::getOptionalValueFromJson<std::string>(json, RomInfo::TITLE_JSON_FIELD);
    // Finding rom year
    info.year = utils::getOptionalValueFromJson<std::string>(json, RomInfo::YEAR_JSON_FIELD);
    // Finding rom manufacturer
    info.manufacturer = utils::getOptionalValueFromJson<std::string>(json, RomInfo::MANUFACTURER_JSON_FIELD);
    // Finding if rom is a bios
    info.isBios = utils::getOptionalValueFromJson<bool>(json, RomInfo::ISBIOS_JSON_FIELD);
}
