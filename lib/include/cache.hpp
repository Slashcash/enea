#ifndef CACHE_HPP
#define CACHE_HPP

#include <filesystem>
#include <list>
#include <optional>

#include "rom.hpp"

class Cache
{
 public:
    enum class Error
    {
        UNABLE_TO_PARSE_CACHE,
        INCOMPATIBLE_CACHE_VERSION,
        UNABLE_TO_WRITE_CACHE_FILE
    };

    class Excep : public Exception
    {
        using Exception::Exception;
    };

 private:
    static constexpr std::string_view VERSION_FIELD = "version";
    static constexpr std::string_view ROMSOURCE_FIELD = "romSources";
    static constexpr std::string_view PATH_FIELD = "path";
    static constexpr std::string_view LASTMODIFIED_FIELD = "lastModified";
    static constexpr std::string_view ROMS_FIELD = "roms";

    std::filesystem::path mPath;
    std::string mRomFolderLastModification;
    std::filesystem::path mRomPath;
    bool mLoaded{false};
    std::list<Rom> mRoms;

    [[nodiscard]] virtual std::string loadCacheFile(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<Error> writeCacheFile(const std::filesystem::path& path,
                                                              const std::string& content) const;

 public:
    Cache() = delete;
    Cache(const Cache& cache) = delete;
    Cache(Cache&& cache) = delete;
    explicit Cache(std::filesystem::path path);

    [[nodiscard]] std::filesystem::path path() const;
    [[nodiscard]] bool loaded() const;
    [[nodiscard]] std::optional<Error> load();
    [[nodiscard]] std::optional<Error> write() const;
    [[nodiscard]] std::optional<std::list<Rom>> roms() const;
    [[nodiscard]] std::optional<std::string> lastEditTime() const;
    void setRomCache(const std::filesystem::path& path, const std::list<Rom>& roms, const std::string& lastEditTime);

    Cache& operator=(const Cache& cache) = delete;
    Cache& operator=(Cache&& cache) = delete;

    virtual ~Cache() = default;
};

#endif // CACHE_HPP
