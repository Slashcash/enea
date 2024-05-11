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
#include "romdb.hpp"

class Rom
{
 private:
    std::filesystem::path mPath;
    RomDB::RomInfo mInfo;
    std::optional<std::filesystem::path> mMedia;

 public:
    class Excep : public Exception
    {
        using Exception::Exception;
    };

    Rom() = delete;
    explicit Rom(std::filesystem::path path);
    explicit Rom(std::filesystem::path path, RomDB::RomInfo info);

    [[nodiscard]] std::filesystem::path path() const;
    [[nodiscard]] std::optional<std::string> title() const;
    [[nodiscard]] std::optional<std::string> year() const;
    [[nodiscard]] std::optional<std::string> manufacturer() const;
    [[nodiscard]] std::optional<std::filesystem::path> media() const;
    [[nodiscard]] std::optional<bool> isBios() const;

    void setMedia(const std::filesystem::path& media);

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
            throw Rom::Excep("Rom constructed from invalid json");
        }

        RomDB::RomInfo romInfo;

        // Finding rom title
        try
        {
            romInfo.title = jsonv.at("title").get<std::string>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            romInfo.title.reset();
        }

        // Finding rom year
        try
        {
            romInfo.year = jsonv.at("year").get<std::string>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            romInfo.year.reset();
        }

        // Finding rom manufacturer
        try
        {
            romInfo.manufacturer = jsonv.at("manufacturer").get<std::string>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            romInfo.manufacturer.reset();
        }

        // Finding if rom is a bios
        try
        {
            romInfo.isBios = jsonv.at("isBios").get<bool>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            romInfo.isBios.reset();
        }

        Rom rom{romPath, romInfo};

        // Finding if rom has a media associated to it
        std::optional<std::filesystem::path> media;
        try
        {
            rom.setMedia(jsonv.at("media").get<std::filesystem::path>());
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

        // Setting rom title
        if (auto title = rom.title(); title)
        {
            jsonv["title"] = *title;
        }

        // Setting rom year
        if (auto year = rom.year(); year)
        {
            jsonv["year"] = *year;
        }

        // Setting rom manufacturer
        if (auto manufacturer = rom.manufacturer(); manufacturer)
        {
            jsonv["manufacturer"] = *manufacturer;
        }

        // Setting if it is bios
        if (auto isBios = rom.isBios(); isBios)
        {
            jsonv["isBios"] = *isBios;
        }

        // Setting if there is any media associated to it
        if (auto media = rom.media(); media)
        {
            jsonv["media"] = *media;
        }
    }
};
} // namespace nlohmann

#endif // ROM_HPP
