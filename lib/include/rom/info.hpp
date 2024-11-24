#ifndef ROMINFO_HPP
#define ROMINFO_HPP

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "utils.hpp"

namespace Rom {
struct Info
{
    static constexpr std::string_view TITLE_JSON_FIELD = "title";
    static constexpr std::string_view YEAR_JSON_FIELD = "year";
    static constexpr std::string_view MANUFACTURER_JSON_FIELD = "manufacturer";
    static constexpr std::string_view ISBIOS_JSON_FIELD = "isBios";

    std::string title;
    std::optional<std::string> year;
    std::optional<std::string> manufacturer;
    std::optional<bool> isBios;

    [[nodiscard]] inline std::string toString() const
    {
        return title;
    }

    bool operator==(const Info&) const = default;
};

inline void to_json(nlohmann::json& json, const Rom::Info& info)
{
    json.clear();

    json[Rom::Info::TITLE_JSON_FIELD] = info.title;
    utils::addOptionalToJson(json, Rom::Info::YEAR_JSON_FIELD, info.year);
    utils::addOptionalToJson(json, Rom::Info::MANUFACTURER_JSON_FIELD, info.manufacturer);
    utils::addOptionalToJson(json, Rom::Info::ISBIOS_JSON_FIELD, info.isBios);
}

inline void from_json(const nlohmann::json& json, Rom::Info& info)
{
    info.title = json.at(Rom::Info::TITLE_JSON_FIELD).get<std::string>();
    info.year = utils::getOptionalValueFromJson<std::string>(json, Rom::Info::YEAR_JSON_FIELD);
    info.manufacturer = utils::getOptionalValueFromJson<std::string>(json, Rom::Info::MANUFACTURER_JSON_FIELD);
    info.isBios = utils::getOptionalValueFromJson<bool>(json, Rom::Info::ISBIOS_JSON_FIELD);
}
} // namespace Rom

template <> struct fmt::formatter<Rom::Info> : fmt::formatter<string_view>
{
    auto format(const Rom::Info& romInfo, fmt::format_context& ctx) const -> fmt::format_context::iterator
    {
        return fmt::formatter<string_view>::format(romInfo.toString(), ctx);
    }
};

#endif // ROMINFO_HPP
