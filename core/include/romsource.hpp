#ifndef ROMSOURCE_HPP
#define ROMSOURCE_HPP

#include <filesystem>
#include <list>
#include <optional>
#include <stdexcept>
#include <system_error>
#include <thread>

#include <nlohmann/json.hpp>
#include <rocket.hpp>

#include "romdb.hpp"

class Rom;

class RomSource
{
 public:
    enum class Error
    {
        INVALID_PATH,
        INVALID_CACHE_FILE,
        ERROR_WRITE_FILE,
        DESYNCED_CACHE_FILE,
        UNABLE_TO_READ_LAST_EDIT_TIME,
        ALREADY_MONITORED
    };

 private:
    std::unique_ptr<std::jthread> mScanThread;
    mutable std::mutex mListMutex;
    std::filesystem::path mPath;
    std::list<Rom> mRoms; // ACCESSING THIS SHOULD BE THREAD SAFE
    bool mMonitored = false;
    RomDB mRomdb;

    [[nodiscard]] virtual std::optional<std::error_code> folderExists(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::list<std::filesystem::path> scanFolder(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<std::error_code> createFolders(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::pair<std::optional<Error>, nlohmann::json>
    readCache(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<Error> writeCache(const nlohmann::json& j,
                                                          const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::pair<std::optional<Error>, std::string>
    lastCacheModification(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<RomDB::RomInfo> findInDB(const std::string& romName) const;
    void fillFromFolder(const std::filesystem::path& cachePath);
    void produceCacheFile(const std::filesystem::path& cachePath) const;

 public:
    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    RomSource() = delete;
    explicit RomSource(const std::filesystem::path& path);

    [[nodiscard]] inline std::filesystem::path path() const
    {
        return mPath;
    };

    [[nodiscard]] inline std::list<Rom> romList() const
    {
        std::scoped_lock lock(mListMutex);
        return mRoms;
    };

    void waitPendingOperations();
    [[nodiscard]] std::optional<Error> monitor();

    virtual ~RomSource();

    mutable rocket::signal<void(const Rom& rom)> romAdded;
    mutable rocket::signal<void(const Rom& rom)> romDeleted;
};

#endif // ROMSOURCE_HPP