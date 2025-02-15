#ifndef ROMGAME_HPP
#define ROMGAME_HPP

#include <filesystem>
#include <fmt/format.h>
#include <map>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

#include "database/table.hpp"
#include "exception.hpp"
#include "rom/info.hpp"
#include "rom/media.hpp"
#include "utils.hpp"

namespace Rom {
class Game
{
 private:
    std::filesystem::path mPath;
    Rom::Info mInfo;
    std::optional<Rom::Media> mMedia;

 public:
    static constexpr std::string_view PATH_JSON_FIELD = "path";
    static constexpr std::string_view INFO_JSON_FIELD = "info";
    static constexpr std::string_view MEDIA_JSON_FIELD = "media";

    Game() = delete;
    explicit Game(const std::filesystem::path& path, const Rom::Info& info,
                  const std::optional<Rom::Media>& media = std::nullopt);

    [[nodiscard]] std::filesystem::path path() const;
    [[nodiscard]] Rom::Info info() const;
    [[nodiscard]] std::optional<Rom::Media> media() const;

    [[nodiscard]] std::string toString() const;

    [[nodiscard]] bool operator==(const Game& game) const;
};

static constexpr char dbPath[] = "romdb/romdb.json";
using Database = Database::Table<std::string, Rom::Info, dbPath>;
} // namespace Rom

namespace nlohmann {
template <> struct adl_serializer<Rom::Game>
{
    static Rom::Game from_json(const json& json)
    {
        auto romPath = json.at(Rom::Game::PATH_JSON_FIELD).get<std::filesystem::path>();
        auto romInfo = json.at(Rom::Game::INFO_JSON_FIELD).get<Rom::Info>();
        auto romMedia = utils::getOptionalValueFromJson<Rom::Media>(json, Rom::Game::MEDIA_JSON_FIELD);

        Rom::Game rom(romPath, romInfo, romMedia);

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
        utils::addOptionalToJson<Rom::Media>(json, Rom::Game::MEDIA_JSON_FIELD, rom.media());
    }
};
} // namespace nlohmann

template <> struct fmt::formatter<Rom::Game> : fmt::formatter<string_view>
{
    auto format(const Rom::Game& game, fmt::format_context& ctx) const -> fmt::format_context::iterator
    {
        return fmt::formatter<string_view>::format(game.toString(), ctx);
    }
};

#endif // ROMGAME_HPP
