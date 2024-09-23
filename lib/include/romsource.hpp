#ifndef ROMSOURCE_HPP
#define ROMSOURCE_HPP

#include <filesystem>
#include <optional>
#include <vector>

#include "exception.hpp"
#include "rom.hpp"
#include "rominfo.hpp"

/**
 * @brief This class represents a source folder where to retrieve roms and theirs associated media.
 * It will scan a folder and provide a list of found roms and media. If the folder was not modified since
 * last scan it will use an internal cache in order to not perform an useless scan
 */
class RomSource
{
 private:
    struct ScanResult
    {
        std::vector<std::filesystem::path> roms;
        std::vector<std::filesystem::path> screenshots;
    };

    struct Cache
    {
        std::string lastModified;
        std::vector<Rom> roms;
    };

    std::filesystem::path mRomFolder;
    std::filesystem::path mCacheFolder;
    mutable Cache mCache;

    [[nodiscard]] virtual bool isFolder(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::vector<std::filesystem::path> scanFolder(const std::filesystem::path& folder) const;
    [[nodiscard]] static std::vector<std::filesystem::path> fileInFolder(const std::filesystem::path& folder);
    [[nodiscard]] virtual std::optional<RomInfo> romInfo(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<std::string> lastFolderModification(const std::filesystem::path& path) const;
    [[nodiscard]] virtual bool writeCacheFile(const nlohmann::json& json, const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<nlohmann::json> readCacheFile(const std::filesystem::path& path) const;
    // Instead of directly using projectVersion variable in softwareinfo.hpp we wrap around this virtual so we can test
    // some scenarios
    [[nodiscard]] virtual std::string_view version() const;
    [[nodiscard]] ScanResult scanResult(const std::filesystem::path& path) const;
    [[nodiscard]] std::vector<Rom> physicalScan() const;
    [[nodiscard]] std::optional<std::vector<Rom>> cacheScan(std::string_view lastModified) const;
    [[nodiscard]] std::filesystem::path cacheFile() const;
    [[nodiscard]] static bool fileIsImage(const std::filesystem::path& path);
    [[nodiscard]] static bool fileIsRom(const std::filesystem::path& path);

 public:
    // These are only public to ease testing
    static constexpr std::string_view LASTMODIFIED_JSON_FIELD = "lastModified";
    static constexpr std::string_view ROMS_JSON_FIELD = "roms";
    static constexpr std::string_view VERSION_JSON_FIELD = "version";

    class Excep : public Exception
    {
        using Exception::Exception;
    };

    RomSource() = delete;
    explicit RomSource(std::filesystem::path romFolder, std::filesystem::path cacheFolder);
    RomSource(const RomSource& source) = delete;
    RomSource(RomSource&& source) = delete;

    [[nodiscard]] std::filesystem::path romFolder() const;
    [[nodiscard]] std::filesystem::path cacheFolder() const;
    [[nodiscard]] std::vector<Rom> scan() const;
    [[nodiscard]] bool saveOnCache() const;

    RomSource& operator=(const RomSource& romsource) = delete;
    RomSource& operator=(RomSource&& romsource) = delete;

    virtual ~RomSource() = default;
};

#endif // ROMSOURCE_HPP
