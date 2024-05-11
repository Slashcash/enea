#include "cache.hpp"

#include <fstream>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "softwareinfo.hpp"

Cache::Cache(std::filesystem::path path) : mPath(std::move(path))
{
}

std::string Cache::loadCacheFile(const std::filesystem::path& path) const
{
    std::ifstream cacheFile(path.string());
    std::stringstream buffer;
    buffer << cacheFile.rdbuf();
    return buffer.str();
}

std::filesystem::path Cache::path() const
{
    return mPath;
}

std::optional<std::list<Rom>> Cache::roms() const
{
    return mLoaded ? std::optional<std::list<Rom>>(mRoms) : std::nullopt;
}

std::optional<std::string> Cache::lastEditTime() const
{
    return mLoaded ? std::optional<std::string>(mRomFolderLastModification) : std::nullopt;
}

bool Cache::loaded() const
{
    return mLoaded;
}

std::optional<Cache::Error> Cache::load()
{
    mLoaded = false;

    nlohmann::json jsonv;
    try
    {
        jsonv = nlohmann::json::parse(loadCacheFile(mPath));
    }
    catch (...)
    {
        return Error::UNABLE_TO_PARSE_CACHE;
    }

    // Parsing cache version file, if it is produced with another version of the software we deem it invalid
    auto version = jsonv.find(VERSION_FIELD);
    if (version == jsonv.end())
    {
        return Error::UNABLE_TO_PARSE_CACHE;
    }

    if (*version != projectVersion)
    {
        return Error::INCOMPATIBLE_CACHE_VERSION;
    }

    // Parsing the rom source, if there is no romsource field we deem the cache file invalid
    auto romSource = jsonv.find(ROMSOURCE_FIELD);
    if (romSource == jsonv.end())
    {
        return Error::UNABLE_TO_PARSE_CACHE;
    }

    // Finding the path of the rom source, if there is no path we deem the cache file invalid
    auto romFolderPath = romSource->find(PATH_FIELD);
    if (romFolderPath == romSource->end())
    {
        return Error::UNABLE_TO_PARSE_CACHE;
    }

    mRomPath = std::string(*romFolderPath);

    // Finding the last modification of the rom source folder, we will compare with the folder current value.
    // If they don't match it means that the folder has been modified in the meanwhile and it needs a rescan.
    auto lastModifiedSaved = romSource->find(LASTMODIFIED_FIELD);
    if (lastModifiedSaved == romSource->end())
    {
        return Error::UNABLE_TO_PARSE_CACHE;
    }

    mRomFolderLastModification = *lastModifiedSaved;

    auto roms = romSource->find(ROMS_FIELD);
    if (roms == romSource->end())
    {
        return Error::UNABLE_TO_PARSE_CACHE;
    }

    for (const auto& rom : *roms)
    {
        try
        {
            mRoms.emplace_back(rom);
        }
        catch ([[maybe_unused]] const Rom::Excep& exception)
        {
            // Just skipping this rom if unable to construct it
        }
    }

    mLoaded = true;
    return std::nullopt;
}

std::optional<Cache::Error> Cache::write() const
{
    nlohmann::json jsonv;
    jsonv[VERSION_FIELD] = projectVersion;
    jsonv[ROMSOURCE_FIELD][PATH_FIELD] = mRomPath;
    jsonv[ROMSOURCE_FIELD][LASTMODIFIED_FIELD] = mRomFolderLastModification;

    nlohmann::json romArray;
    for (const auto& rom : mRoms)
    {
        romArray.push_back(rom);
    }

    jsonv[ROMSOURCE_FIELD][ROMS_FIELD] = romArray;

    // Nicely formatting our json
    std::stringstream stringStream;
    stringStream << std::setw(4) << jsonv << std::endl;

    if (auto error = writeCacheFile(mPath, stringStream.str()); error.has_value())
    {
        return error;
    }

    return std::nullopt;
}

std::optional<Cache::Error> Cache::writeCacheFile(const std::filesystem::path& path, const std::string& content) const
{
    std::ofstream file(path.string().c_str());
    file << content;

    if (!file)
    {
        return Error::UNABLE_TO_WRITE_CACHE_FILE;
    }

    return std::nullopt;
}

void Cache::setRomCache(const std::filesystem::path& path, const std::list<Rom>& roms, const std::string& lastEditTime)
{
    mRomPath = path;
    mRoms = roms;
    mRomFolderLastModification = lastEditTime;
    mLoaded = true;
}
