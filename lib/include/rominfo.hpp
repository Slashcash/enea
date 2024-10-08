#ifndef ROMINFO_HPP
#define ROMINFO_HPP

#include <nlohmann/json.hpp>
/**
 * @brief This struct represents a set of information about a rom. This information
 * is generally retrieved from a rom database (currently the database is stored as a json format)
 * within the source code, please see: db/romdb.json for reference.
 */
struct RomInfo
{
    static constexpr std::string_view TITLE_JSON_FIELD = "title";
    static constexpr std::string_view YEAR_JSON_FIELD = "year";
    static constexpr std::string_view MANUFACTURER_JSON_FIELD = "manufacturer";
    static constexpr std::string_view ISBIOS_JSON_FIELD = "isBios";

    std::optional<std::string> title;
    std::optional<std::string> year;
    std::optional<std::string> manufacturer;
    std::optional<bool> isBios;
};

void to_json(nlohmann::json& json, const RomInfo& info);
void from_json(const nlohmann::json& json, RomInfo& info);

#endif // ROMINFO_HPP
