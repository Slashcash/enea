#ifndef ROMGAME_HPP
#define ROMGAME_HPP

#include <filesystem>
#include <map>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

#include "database.hpp"
#include "exception.hpp"
#include "rom/info.hpp"
#include "rom/media.hpp"

namespace Rom {
/**
 * @brief This struct represents a game rom within the context of this software. Roms are meant to be
 * launched by AdvMAME and are generally identified by their path on the filesystem.
 * This class also stores information related to roms and the medias associated to it.
 */
class Game
{
 private:
    std::filesystem::path mPath;
    // This info should really become an optional
    Rom::Info mInfo;
    Rom::Media mMedia;

 public:
    static constexpr std::string_view PATH_JSON_FIELD = "path";
    static constexpr std::string_view INFO_JSON_FIELD = "info";
    static constexpr std::string_view MEDIA_JSON_FIELD = "media";

    class Exception : public enea::Exception
    {
        using enea::Exception::Exception;
    };

    Game() = delete;
    explicit Game(std::filesystem::path path);
    explicit Game(std::filesystem::path path, Rom::Info info, Rom::Media media);

    [[nodiscard]] std::filesystem::path path() const;
    [[nodiscard]] Rom::Info info() const;
    [[nodiscard]] Rom::Media media() const;

    [[nodiscard]] bool operator==(const Game& game) const;
};

static constexpr char dbPath[] = "romdb/romdb.json";
using Database = DB<std::string, Rom::Info, dbPath>;
} // namespace Rom

namespace nlohmann {
template <> struct adl_serializer<Rom::Game>
{
    static Rom::Game from_json(const json& json)
    {
        // Finding rom path
        std::filesystem::path romPath;
        try
        {
            json.at(Rom::Game::PATH_JSON_FIELD).get_to(romPath);
        }
        catch (const json::exception& e)
        {
            throw Rom::Game::Exception("Rom constructed from invalid json, mandatory path field is missing");
        }

        // Finding rom info
        Rom::Info romInfo;
        try
        {
            romInfo = json.at(Rom::Game::INFO_JSON_FIELD).get<Rom::Info>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            // Nothing happening if it is not present as this field is optional
        }

        // Finding rom media
        Rom::Media romMedia;
        try
        {
            romMedia = json.at(Rom::Game::MEDIA_JSON_FIELD).get<Rom::Media>();
        }
        catch ([[maybe_unused]] const json::exception& e)
        {
            // Do not do anything, no media is allowed
        }

        Rom::Game rom{romPath, romInfo, romMedia};

        return rom;
    }

    static void to_json(json& json, Rom::Game const& rom)
    {
        json.clear();

        // Setting rom path
        json[Rom::Game::PATH_JSON_FIELD] = rom.path();

        // Setting rom info
        json[Rom::Game::INFO_JSON_FIELD] = rom.info();

        // Setting rom media
        json[Rom::Game::MEDIA_JSON_FIELD] = rom.media();
    }
};
} // namespace nlohmann

#endif // ROMGAME_HPP
