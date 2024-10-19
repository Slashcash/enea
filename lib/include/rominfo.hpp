#ifndef ROMINFO_HPP
#define ROMINFO_HPP

#include <nlohmann/json.hpp>
/**
 * @brief This struct represents a set of information about a rom. This information
 * is generally retrieved from a rom database (currently the database is stored as a json format
 * within the source code), please see: db/romdb.json for reference.
 */
struct RomInfo
{
    /**
     * @brief The label for the rom title in the json file
     *
     */
    static constexpr std::string_view TITLE_JSON_FIELD = "title";
    /**
     * @brief The label for the rom year in the json file
     *
     */
    static constexpr std::string_view YEAR_JSON_FIELD = "year";
    /**
     * @brief The label for the rom manufacturer in the json file
     *
     */
    static constexpr std::string_view MANUFACTURER_JSON_FIELD = "manufacturer";
    /**
     * @brief The label for the information abouth whether the rom is bios or not in the json file
     *
     */
    static constexpr std::string_view ISBIOS_JSON_FIELD = "isBios";

    /**
     * @brief The rom title. It is supposed to contain the full title of the rom.
     * eg: the ssf2t.zip rom title will be 'Super Street Fighter II Turbo'. It is an optional
     * because this information may not be available for a rom (for example it may be absent from the database)
     *
     */
    std::optional<std::string> title;
    /**
     * @brief The rom release year.
     * eg: the ssf2t.zip rom release year will be '1994'. It is an optional
     * because this information may not be available for a rom (for example it may be absent from the database)
     *
     */
    std::optional<std::string> year;
    /**
     * @brief The rom manufacturer (generally speaking whoever developed the game).
     * eg: the ssf2t.zip rom manufacturer will be 'Capcom'. It is an optional
     * because this information may not be available for a rom (for example it may be absent from the database)
     *
     */
    std::optional<std::string> manufacturer;
    /**
     * @brief Whether the rom is a bios. In the MAME implementation some roms
     * are not really playable roms per se. They are BIOS roms and are needed to launch some
     * other games.
     * eg: the mslug.zip rom needs the neogeo.zip. This bios roms cannot be launched
     * as normal games and will ideally not appear to the end user of the frontend.
     * It is an optional because this information may not be available for a rom (for example it may be absent from the
     * database)
     *
     */
    std::optional<bool> isBios;
};

/**
 * @brief nlohmann::json helper function. It is used by the nlohmann::json
 * library in order to automatically convert the RomInfo struct to a json.
 *
 * @param json the output json
 * @param info the input RomInfo struct
 */
void to_json(nlohmann::json& json, const RomInfo& info);
/**
 * @brief nlohmann::json helper function. It is used by the nlohmann::json
 * library in order to automatically obtain a RomInfo from a json.
 *
 * @param json the input json
 * @param info the output RomInfo struct
 */
void from_json(const nlohmann::json& json, RomInfo& info);

#endif // ROMINFO_HPP
