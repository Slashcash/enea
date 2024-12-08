#ifndef ROMSOURCE_HPP
#define ROMSOURCE_HPP

#include "model.hpp"
#include "rom/game.hpp"
#include "softwareinfo.hpp"

namespace Rom {

class Source : public Model<Game>
{
 private:
    std::string mIdentifier;
    std::once_flag mMonitorCalled;
    bool mMonitored = false;
    mutable std::optional<std::string> mLastModified;
    std::filesystem::path mCacheFile;

    [[nodiscard]] virtual std::vector<std::filesystem::path> scan() const = 0;
    [[nodiscard]] virtual std::vector<Game> parse() const final;
    [[nodiscard]] virtual std::optional<std::vector<Game>> cache() const final;
    [[nodiscard]] virtual std::optional<std::string> lastModified() const = 0;
    [[nodiscard]] virtual std::optional<Rom::Info> romInfo(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<nlohmann::json> readCacheFile(const std::filesystem::path& path) const;
    [[nodiscard]] virtual bool writeCacheFile(const nlohmann::json& json, const std::filesystem::path& path) const;
    [[nodiscard]] virtual inline std::string_view version() const
    {
        return projectVersion;
    } // just here so we can test some scenarios

 public:
    static constexpr std::string_view VERSION_JSON_FIELD = "version";
    static constexpr std::string_view ROMS_JSON_FIELD = "roms";
    static constexpr std::string_view LASTMODIFIED_JSON_FIELD = "lastModified";

    explicit inline Source(const std::string& identifier, const std::filesystem::path& cacheFolder)
        : mIdentifier(identifier),
          mCacheFile(cacheFolder /
                     fmt::format("{}{}", std::to_string(std::filesystem::hash_value(identifier)), ".json"))
    {
    }

    virtual void monitor() final;
    [[nodiscard]] virtual bool writeCache() const final;

    virtual ~Source() = default;
};
} // namespace Rom

#endif // ROMSOURCE_HPP
