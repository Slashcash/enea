#ifndef ROM_HPP
#define ROM_HPP

#include <filesystem>
#include <map>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

#include "exception.hpp"
#include "rominfo.hpp"
#include "rommedia.hpp"

/**
 * @brief This struct represents a game rom within the context of this software. Roms are meant to be
 * launched by AdvMAME and are generally identified by their path on the filesystem.
 * This class also stores information related to roms and the medias associated to it.
 */
class Rom
{
 private:
    std::filesystem::path mPath;
    RomInfo mInfo;
    RomMedia mMedia;

 public:
    static constexpr std::string_view PATH_JSON_FIELD = "path";
    static constexpr std::string_view INFO_JSON_FIELD = "info";
    static constexpr std::string_view MEDIA_JSON_FIELD = "media";

    class Excep : public Exception
    {
        using Exception::Exception;
    };

    Rom() = delete;
    explicit Rom(std::filesystem::path path);
    explicit Rom(std::filesystem::path path, RomInfo info, RomMedia media);

    [[nodiscard]] std::filesystem::path path() const;

    [[nodiscard]] RomInfo info() const;
    [[nodiscard]] std::optional<std::string> title() const;
    [[nodiscard]] std::optional<std::string> year() const;
    [[nodiscard]] std::optional<std::string> manufacturer() const;
    [[nodiscard]] std::optional<bool> isBios() const;

    [[nodiscard]] RomMedia media() const;
    [[nodiscard]] std::optional<std::filesystem::path> screenshot() const;

    [[nodiscard]] bool operator==(const Rom& rom) const;
};

namespace nlohmann {
template <> struct adl_serializer<Rom>
{
    static Rom from_json(const json& json)
    {
        // Finding rom path
        std::filesystem::path romPath;
        try
        {
            json.at(Rom::PATH_JSON_FIELD).get_to(romPath);
        }
        catch (const json::exception& e)
        {
            throw Rom::Excep("Rom constructed from invalid json, mandatory path field is missing");
        }

        // Finding rom info
        RomInfo romInfo;
        try
        {
            romInfo = json.at(Rom::INFO_JSON_FIELD).get<RomInfo>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            // Nothing happening if it is not present as this field is optional
        }

        // Finding rom media
        RomMedia romMedia;
        try
        {
            romMedia = json.at(Rom::MEDIA_JSON_FIELD).get<RomMedia>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            // Do not do anything, no media is allowed
        }

        Rom rom{romPath, romInfo, romMedia};

        return rom;
    }

    static void to_json(json& json, Rom const& rom)
    {
        json.clear();

        // Setting rom path
        json[Rom::PATH_JSON_FIELD] = rom.path();

        // Setting rom info
        json[Rom::INFO_JSON_FIELD] = rom.info();

        // Setting rom media
        json[Rom::MEDIA_JSON_FIELD] = rom.media();
    }
};
} // namespace nlohmann

#endif // ROM_HPP
