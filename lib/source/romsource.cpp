#include "romsource.hpp"

#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <spdlog/spdlog.h>

#include "database.hpp"
#include "rom/media.hpp"
#include "softwareinfo.hpp"

RomSource::RomSource(std::filesystem::path romFolder, std::filesystem::path cacheFolder)
    : mRomFolder(std::move(romFolder)), mCacheFolder(std::move(cacheFolder))
{
}

std::filesystem::path RomSource::romFolder() const
{
    return mRomFolder;
}

std::filesystem::path RomSource::cacheFolder() const
{
    return mCacheFolder;
}

std::vector<Rom::Game> RomSource::scan() const
{
    std::vector<Rom::Game> result;

    if (!isFolder(mRomFolder))
    {
        throw RomSource::Exception("Scan path is not a folder");
    }

    spdlog::debug("Getting last modification time for: {}", mRomFolder.string());
    auto lastModification = lastFolderModification(mRomFolder);

    // If unable to get the folder last modification date we throw
    if (!lastModification)
    {
        throw RomSource::Exception(fmt::format("Unable to get last folder modification for: {}", mRomFolder.string()));
    }

    // If unable to load data from cache we phisycally scan the folder
    auto cache = cacheScan(*lastModification);
    cache ? result = *cache : result = physicalScan();

    // Storing cache
    mCache.roms = result;
    mCache.lastModified = *lastModification;

    return result;
}

std::vector<Rom::Game> RomSource::physicalScan() const
{
    std::vector<Rom::Game> result;
    spdlog::debug("Searching for roms and media in: {}", mRomFolder.string());
    // Physically scanning the folder
    for (auto files = scanResult(mRomFolder); const auto& rom : files.roms)
    {
        if (auto query = romInfo(rom); !query || (query->isBios && *(query->isBios)))
        {
            spdlog::warn("File: {} does not look like a launchable rom, discarding it", rom.string());
        }
        else
        {
            spdlog::trace("Found rom: {}", rom.string());
            Rom::Media media;

            // Searching if there is an image file with the stem equal to the rom's one, in this case it's a screenshot
            if (auto picture = std::ranges::find_if(files.screenshots,
                                                    [&rom](const std::filesystem::path& screenshot) {
                                                        return fileIsImage(screenshot) &&
                                                               screenshot.stem() == rom.stem();
                                                    });
                picture != files.screenshots.end())

            {
                spdlog::trace("Found screenshot: {}", picture->string());
                media.screenshot = *picture;
            }

            result.emplace_back(rom, *query, media);
        }
    }

    return result;
}

bool RomSource::isFolder(const std::filesystem::path& path) const
{
    return std::filesystem::is_directory(mRomFolder);
}

std::vector<std::filesystem::path> RomSource::scanFolder(const std::filesystem::path& folder) const
{
    std::vector<std::filesystem::path> result;
    auto entries = fileInFolder(folder);
    for (const auto& entry : entries)
    {
        if (std::filesystem::is_regular_file(entry))
        {
            result.push_back(std::filesystem::absolute(entry));
        }
    }

    return result;
}

RomSource::ScanResult RomSource::scanResult(const std::filesystem::path& path) const
{
    ScanResult result;

    for (auto files = scanFolder(path); const auto& file : files)
    {
        if (fileIsRom(file))
        {
            result.roms.emplace_back(file);
        }

        else if (fileIsImage(file))
        {
            result.screenshots.emplace_back(file);
        }

        else
        {
            spdlog::warn("File: {} does not look like a rom or an image, discarding it", file.string());
        }
    }

    return result;
}

bool RomSource::fileIsImage(const std::filesystem::path& path)
{
    return path.extension() == ".png" || path.extension() == ".jpeg" || path.extension() == ".jpg";
}

bool RomSource::fileIsRom(const std::filesystem::path& path)
{
    return path.extension() == ".zip";
}

std::optional<Rom::Info> RomSource::romInfo(const std::filesystem::path& path) const
{
    spdlog::trace("Querying rom database for: {}", path.stem().string());
    return Rom::Database::get().find(path.stem().string());
}

std::optional<std::string> RomSource::lastFolderModification(const std::filesystem::path& path) const
{
    std::error_code errorCode;

    // With this algorithm we scan all the subfolders of a folder and we find the most recently modified one, this way
    // we know which is the very last modification moment of a folder structure
    std::vector<std::filesystem::file_time_type> editTimes;

    // First checking the modification time of the actual folder
    std::filesystem::file_time_type lastModified = std::filesystem::last_write_time(path, errorCode);
    if (!errorCode)
    {
        editTimes.push_back(lastModified);
    }

    // Then we scan all the subfolders for edit times
    auto entries = fileInFolder(path);
    for (const auto& entry : entries)
    {
        if (lastModified = std::filesystem::last_write_time(entry, errorCode);
            std::filesystem::is_directory(entry) && !errorCode)
        {
            editTimes.push_back(lastModified);
        }
    }

    if (editTimes.empty())
    {
        return std::nullopt;
    }

    // Then we find the most recent modification
    lastModified = *(std::ranges::max_element(editTimes));

    // I can't believe it's 2024 and I still need to do this cumbersome stuff.
    // Unluckily I can't use std::format since my GCC doesn't support it yet.
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastModified - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    auto scpt_time_t = std::chrono::system_clock::to_time_t(sctp);
    std::tm* gmt = std::gmtime(&scpt_time_t);
    std::stringstream timess;
    timess << std::put_time(gmt, "%Y-%m-%d %H:%M:%S");

    return timess.str();
}

bool RomSource::saveOnCache() const
{
    auto file = cacheFile();
    spdlog::debug("Writing rom source cache file at: {}", file.string());

    nlohmann::json json;

    json[LASTMODIFIED_JSON_FIELD] = mCache.lastModified;
    json[VERSION_JSON_FIELD] = version();

    nlohmann::json roms;
    for (const auto& rom : mCache.roms)
    {
        roms.push_back(rom);
    }

    json[ROMS_JSON_FIELD] = roms;

    return writeCacheFile(json, cacheFile());
}

std::filesystem::path RomSource::cacheFile() const
{
    return mCacheFolder / fmt::format("{}{}", std::to_string(std::filesystem::hash_value(mRomFolder)), ".json");
}

bool RomSource::writeCacheFile(const nlohmann::json& json, const std::filesystem::path& path) const
{
    // Nicely formatting our json
    std::stringstream stringStream;
    stringStream << std::setw(4) << json << std::endl;

    std::ofstream file(path.string().c_str());
    file << stringStream.str();

    return file.good();
}

std::optional<nlohmann::json> RomSource::readCacheFile(const std::filesystem::path& path) const
{
    std::ifstream cacheFile(path.string());
    std::stringstream buffer;
    buffer << cacheFile.rdbuf();

    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(buffer.str());
    }
    catch (...)
    {
        return std::nullopt;
    }

    return json;
}

std::optional<std::vector<Rom::Game>> RomSource::cacheScan(const std::string_view lastModified) const
{
    std::vector<Rom::Game> result;

    spdlog::debug("Trying to read cache file at: {}", cacheFile().string());
    auto cache = readCacheFile(cacheFile());

    // If unable to read cache from file we return empty result
    if (!cache)
    {
        spdlog::warn("Unable to read cache file at: {}", cacheFile().string());
        return std::nullopt;
    }

    // If unable to make sure that the cache json was produced by the very same software version that is running
    // right now we return empty result
    try
    {
        if (!cache->contains(VERSION_JSON_FIELD) || cache->at(VERSION_JSON_FIELD).get<std::string>() != version())
        {
            spdlog::warn("Cache file at {} does not seem to be produced by the same software version",
                         cacheFile().string());
            return std::nullopt;
        }
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        spdlog::warn("Cache file at {} does not seem to be produced by the same software version",
                     cacheFile().string());
        return std::nullopt;
    }

    // If unable to make sure that the rom folder was not modified since last write we return empty result
    try
    {
        if (!cache->contains(LASTMODIFIED_JSON_FIELD) ||
            cache->at(LASTMODIFIED_JSON_FIELD).get<std::string>() != lastModified)
        {
            spdlog::warn("The rom folder was probably modified since last scan");
            return std::nullopt;
        }
    }
    catch ([[maybe_unused]] const nlohmann::json::exception& e)
    {
        spdlog::warn("The rom folder was probably modified since last scan");
        return std::nullopt;
    }

    if (!cache->contains(ROMS_JSON_FIELD))
    {
        spdlog::warn("Cache seems to be malformed or corrupted");
        return std::nullopt;
    }

    auto roms = (*cache)[ROMS_JSON_FIELD];
    for (const auto& rom : roms)
    {
        const auto& inserted = result.emplace_back(rom);
        spdlog::trace("Found rom: {}", inserted.path().stem().string());
    }

    return result;
}

std::string_view RomSource::version() const
{
    return projectVersion;
}

std::vector<std::filesystem::path> RomSource::fileInFolder(const std::filesystem::path& folder)
{
    /* We provide two different implementations to iterate over the content of a folder:
        1. The first one uses POSIX API
        2. The second one uses std::filesystem::recursive_directory_iterator

        For some reasons our toolchain adds some strange symbols when using recursive_directory_iterator.
        These symbols are generally unavailable on raspbian (?) and this makes the executable unusable.
        For this reason we use the POSIX API when compiling arm so we don't break raspbian compatibility.
        This issue should be investigated.
    */
    std::vector<std::filesystem::path> result;

#ifdef USE_POSIX_FILE_LIST
    DIR* dp;
    struct dirent* entry;
    struct stat info;

    if ((dp = opendir(folder.c_str())) == NULL)
    {
        return result;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        std::filesystem::path path = folder / entry->d_name;

        if (stat(path.c_str(), &info) != 0)
        {
            continue;
        }

        if (S_ISDIR(info.st_mode))
        {
            // Skip "." and ".." directories
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                // Recurse into subdirectories
                std::vector<std::filesystem::path> subDirFiles = fileInFolder(path);
                result.insert(result.end(), subDirFiles.begin(), subDirFiles.end());
            }
        }
        else
        {
            // It's a file, add to list
            result.push_back(path);
        }
    }

    closedir(dp);
#else
    for (const auto& entry : std::filesystem::recursive_directory_iterator(folder))
    {
        result.push_back(std::filesystem::absolute(entry.path()));
    }
#endif

    return result;
}
