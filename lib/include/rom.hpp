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

class Rom
{
 private:
    std::filesystem::path mPath;
    RomInfo mInfo;
    RomMedia mMedia;

 public:
    class Excep : public Exception
    {
        using Exception::Exception;
    };

    Rom() = delete;
    explicit Rom(std::filesystem::path path);
    explicit Rom(std::filesystem::path path, RomInfo info);

    [[nodiscard]] std::filesystem::path path() const;
    [[nodiscard]] RomInfo info() const;
    [[nodiscard]] std::optional<std::string> title() const;
    [[nodiscard]] std::optional<std::string> year() const;
    [[nodiscard]] std::optional<std::string> manufacturer() const;
    [[nodiscard]] std::optional<bool> isBios() const;

    [[nodiscard]] RomMedia media() const;
    [[nodiscard]] std::optional<std::filesystem::path> screenshot() const;
    void setMedia(const RomMedia& media);

    [[nodiscard]] bool operator==(const Rom& rom) const;
};

namespace nlohmann {
template <> struct adl_serializer<Rom>
{
    static Rom from_json(const json& jsonv)
    {
        // Finding rom path
        std::filesystem::path romPath;
        try
        {
            jsonv.at("path").get_to(romPath);
        }
        catch (const json::exception& e)
        {
            throw Rom::Excep("Rom constructed from invalid json, mandatory path field is missing");
        }

        RomInfo romInfo;

        // Finding rom info
        try
        {
            romInfo = jsonv.at("info").get<RomInfo>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            // Nothing happening if it is not present as this field is optional
        }

        Rom rom{romPath, romInfo};

        // Finding rom media
        try
        {
            rom.setMedia(jsonv.at("media").get<RomMedia>());
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            // Do not do anything, no media is allowed
        }

        return rom;
    }

    static void to_json(json& jsonv, Rom const& rom)
    {
        jsonv.clear();

        // Setting rom path
        jsonv["path"] = rom.path();

        // Setting rom info
        jsonv["info"] = rom.info();

        // Setting rom media
        jsonv["media"] = rom.media();
    }
};
} // namespace nlohmann

#endif // ROM_HPP
