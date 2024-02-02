#ifndef ROM_HPP
#define ROM_HPP

#include <filesystem>
#include <map>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <stdexcept>
#include <string>

#include "romdb.hpp"

class Rom
{
 private:
    std::filesystem::path mPath;
    RomDB::RomInfo mInfo;

 public:
    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    /**
     * @brief Construct a new Rom object from a filesystem path.
     *
     * The path should reference a rom file that is possibly accessible on the filesytem.
     *
     * @param path Path on the filesystem for the rom file.
     */
    explicit Rom(const std::filesystem::path& path);

    /**
     * @brief Construct a new Rom object from a filesystem path and add additional information.
     *
     * The path should reference a rom file that is possibly accessible on the filesystem.
     * Additional information such as title, year and manufacturer will be added to the rom.
     * Such information will probably come from the rom database (see RomDb class). These database
     * is loaded starting from an externally provided .xml file.
     *
     * @param path Path on the filesystem for the rom file.
     * @param info Additional information about this rom (title, year and manufacturer)
     */
    explicit Rom(const std::filesystem::path& path, const RomDB::RomInfo& info);

    /**
     * @brief Get the filesystem path of the rom.
     *
     * @return The filesystem path of the rom.
     */
    [[nodiscard]] std::filesystem::path path() const;

    /**
     * @brief Get the title of the rom if available.
     *
     * @return The title of the rom if available.
     */
    [[nodiscard]] std::optional<std::string> title() const;

    /**
     * @brief Get the release year of the rom if available.
     *
     * @return The release year of the rom if available.
     */
    [[nodiscard]] std::optional<std::string> year() const;

    /**
     * @brief Get the manufacturer of the rom if available.
     *
     * @return The manufacturer of the rom if available.
     */
    [[nodiscard]] std::optional<std::string> manufacturer() const;

    /**
     * @brief Compare two roms.
     *
     * Two roms are considered the same rom if they share the same path
     * on the filesystem. Even if two roms share the same name they cannot
     * be considered the same.
     *
     * @param rom The rom to be compared to.
     * @return Whether the rom are the same.
     */
    [[nodiscard]] bool operator==(const Rom& rom) const;
};

namespace nlohmann {
template <> struct adl_serializer<Rom>
{
    static Rom from_json(const json& j)
    {
        // Finding rom path
        std::filesystem::path romPath;
        try
        {
            j.at("path").get_to(romPath);
        }
        catch (const json::exception& e)
        {
            throw Rom::Exception("Rom constructed from invalid json");
        }

        RomDB::RomInfo romInfo;

        // Finding rom title
        try
        {
            romInfo.title = j.at("title").get<std::string>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            romInfo.title.reset();
        }

        // Finding rom year
        try
        {
            romInfo.year = j.at("year").get<std::string>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            romInfo.year.reset();
        }

        // Finding rom manufacturer
        try
        {
            romInfo.manufacturer = j.at("manufacturer").get<std::string>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            romInfo.manufacturer.reset();
        }

        return Rom{romPath, romInfo};
    }

    static void to_json(json& j, Rom rom)
    {
        j.clear();

        // Setting rom path
        j["path"] = rom.path();

        // Setting rom title
        if (rom.title().has_value())
        {
            j["title"] = *(rom.title());
        }

        // Setting rom year
        if (rom.year().has_value())
        {
            j["year"] = *(rom.year());
        }

        // Setting rom manufacturer
        if (rom.manufacturer().has_value())
        {
            j["manufacturer"] = *(rom.manufacturer());
        }
    }
};
} // namespace nlohmann

#endif // ROM_HPP