#include "romsource.hpp"

#include <fstream>

#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "configuration.hpp"
#include "rom.hpp"
#include "version.hpp"

RomSource::RomSource(const std::filesystem::path& path) : mPath(path)
{
    if (mPath.is_relative())
    {
        throw Exception("Rom source built with relative path");
    }
}

std::optional<std::error_code> RomSource::folderExists(const std::filesystem::path& path) const
{
    if (std::error_code ec; !std::filesystem::is_directory(path, ec))
    {
        return ec;
    }

    return std::nullopt;
}

std::list<std::filesystem::path> RomSource::scanFolder(const std::filesystem::path& path) const
{
    std::list<std::filesystem::path> result;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            result.push_back(entry.path());
        }
    }

    return result;
}

std::optional<RomSource::Error> RomSource::monitor()
{
    if (auto err = folderExists(mPath); err.has_value())
    {
        return Error::INVALID_PATH;
    }

    // Looking for a cache instead of scanning the folder
    auto cacheDir = Configuration::get().cacheDirectory() / std::to_string(std::filesystem::hash_value(mPath));
    auto cacheFile = cacheDir / "cache.json";
    spdlog::debug("Trying to load rom source cache for {} at {}", mPath.string(), cacheFile.string());

    // Creating cache directory if it does not exists
    if (auto error = createFolders(cacheDir); error.has_value())
    {
        spdlog::warn("Failed to create cache folder at {}, Error: {}", cacheDir.string(), error.value().message());
    }

    // Looking for the cache file
    // If unable to read we scan the folder
    if (auto [readError, result] = readCache(cacheFile); readError.has_value())
    {
        spdlog::debug("Unable to load cache file, will scan folder, Error: {}",
                      magic_enum::enum_name(readError.value()));

        nlohmann::json romArray;
        for (auto paths = scanFolder(mPath); const auto& path : paths)
        {
            try
            {
                Rom rom{path};
                romAdded(rom);
                romArray.push_back(rom);
            }
            catch ([[maybe_unused]] const Rom::Exception& e)
            {
                continue;
            }
        }

        // Creating cache
        mCache.clear();
        mCache["path"] = mPath.string();
        mCache["version"] = projectVersion;
        mCache["roms"] = romArray;

        // Reading folder last modified time, if we fail we do not write the cache file
        auto [lastModifiedError, lastModified] = lastCacheModification(mPath);
        if (lastModifiedError.has_value())
        {
            spdlog::warn("Failed to create cache file at {}, Error: {}", cacheFile.string(),
                         magic_enum::enum_name(lastModifiedError.value()));

            return std::nullopt;
        }
        mCache["lastModified"] = lastModified;

        if (auto writeError = writeCache(mCache, cacheFile); writeError.has_value())
        {
            spdlog::warn("Failed to create cache file at {}, Error: {}", cacheFile.string(),
                         magic_enum::enum_name(writeError.value()));

            return std::nullopt;
        }
    }

    else
    {
        spdlog::debug("Loaded cache file at {}", cacheFile.string());
        auto roms = result["roms"];
        for (const auto& rom : roms)
        {
            romAdded(Rom{rom});
        }
    }

    return std::nullopt;
}

std::pair<std::optional<RomSource::Error>, nlohmann::json> RomSource::readCache(const std::filesystem::path& path) const
{
    std::ifstream file(path.c_str());
    nlohmann::json result;
    if (!file.is_open())
    {
        return std::make_pair(Error::INVALID_CACHE_FILE, nlohmann::json());
    }

    try
    {
        result = nlohmann::json::parse(file);
    }
    catch (...)
    {
        return std::make_pair(Error::INVALID_CACHE_FILE, nlohmann::json());
    }

    // If the cache doesn't contain the lastModified field we deem it invalid
    auto lastModifiedSaved = result.find("lastModified");
    if (lastModifiedSaved == result.end())
    {
        return std::make_pair(Error::INVALID_CACHE_FILE, nlohmann::json());
    }

    // If we can't get the rom source last modified time we deem the cache invalid
    auto [lastModifiedError, lastModified] = lastCacheModification(mPath);
    if (lastModifiedError.has_value())
    {
        return std::make_pair(lastModifiedError.value(), nlohmann::json());
    }

    // If rom source last modification time and the cache saved one do not match we deem the cache invalid
    if (lastModified != *lastModifiedSaved)
    {
        return std::make_pair(Error::DESYNCED_CACHE_FILE, nlohmann::json());
    }

    // If cache doesn't have a "roms" field we deem it invalid
    if (result.find("roms") == result.end())
    {
        return std::make_pair(Error::INVALID_CACHE_FILE, nlohmann::json());
    }

    return std::make_pair(std::nullopt, result);
}

std::optional<RomSource::Error> RomSource::writeCache(const nlohmann::json& j, const std::filesystem::path& path) const
{
    std::ofstream file(path.string().c_str());
    if (!file.is_open())
    {
        return Error::ERROR_WRITE_FILE;
    }

    file << std::setw(4) << j << std::endl;
    if (!file.good())
    {
        return Error::ERROR_WRITE_FILE;
    }

    return std::nullopt;
}

std::optional<std::error_code> RomSource::createFolders(const std::filesystem::path& path) const
{
    std::error_code ec;
    std::filesystem::create_directories(path, ec);
    return ec ? ec : std::optional<std::error_code>();
}

std::pair<std::optional<RomSource::Error>, std::string>
RomSource::lastCacheModification(const std::filesystem::path& path) const
{
    std::error_code ec;
    std::filesystem::file_time_type lastModified = std::filesystem::last_write_time(path, ec);
    if (ec)
    {
        return std::make_pair(Error::UNABLE_TO_READ_LAST_EDIT_TIME, std::string());
    }

    // I can't believe it's 2023 and I still need to do this cumbersome stuff.
    // Unluckily I can't use std::format since my GCC doesn't support it yet.
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastModified - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    auto scpt_time_t = std::chrono::system_clock::to_time_t(sctp);
    std::tm* gmt = std::gmtime(&scpt_time_t);
    std::stringstream timess;
    timess << std::put_time(gmt, "%Y-%m-%d %H:%M:%S");

    return std::make_pair(std::nullopt, timess.str());
}

RomSource::~RomSource()
{
    if (auto roms = mCache.find("roms"); roms != mCache.end())
    {
        for (const auto& rom : *roms)
        {
            romDeleted(Rom(rom));
        }
    }
}