#include "rom/source.hpp"

#include <fstream>

void Rom::Source::monitor()
{
    std::call_once(mMonitorCalled, [this]() {
        mMonitored = true;
        std::string monitorLog(fmt::format(R"(Rom monitor operation on "{}".)", mIdentifier));
        auto cachedRoms = cache();

        auto roms = cachedRoms ? *cachedRoms : [this, &monitorLog]() {
            mLastModified = lastModified();
            if (!mLastModified)
            {
                spdlog::warn("{} Failed to retrieve rom source last modified time, we will not be able to have a cache "
                             "for this source",
                             monitorLog);
            }

            return parse();
        }();

        for (const auto& rom : roms)
        {
            if (rom.info().isLaunchable())
            {
                spdlog::trace(R"({} Found rom "{}")", monitorLog, rom.info().title);
                addElement(rom);
            }
            else
            {
                spdlog::trace(R"({} Rom: "{}" does not look like a launchable a rom, will not be added)", monitorLog,
                              rom);
            }
        }

        spdlog::info("{} Successfully retrieved {} roms", monitorLog, elements().size());
    });
}

std::optional<std::vector<Rom::Game>> Rom::Source::cache() const
{
    std::string cacheLog(fmt::format(R"(Cache retrieval operation from "{}".)", mCacheFile.string()));
    std::vector<Rom::Game> result;

    // Trying to read the json
    auto json = readCacheFile(mCacheFile);
    if (!json)
    {
        return std::nullopt;
    }

    // Checking if cache and software share the same version
    std::string cacheVersion;
    try
    {
        cacheVersion = json->at(VERSION_JSON_FIELD);
    }
    catch (const nlohmann::json::exception& excep)
    {
        spdlog::warn(
            R"({} Failed because "{}" does not look like a well-formed cache. Underlying json parser threw "{}")",
            cacheLog, json->dump(), excep.what());

        return std::nullopt;
    }

    auto softwareVersion = version();
    if (cacheVersion != softwareVersion)
    {
        spdlog::debug(R"({} Failed because cache and software version are different, cache: "{}", software "{}")",
                      cacheLog, cacheVersion, softwareVersion);
    }

    // Trying to retrieve the last modified time
    try
    {
        mLastModified = json->at(LASTMODIFIED_JSON_FIELD);
    }
    catch (const nlohmann::json::exception& excep)
    {
        spdlog::warn(
            R"({} Failed because "{}" does not look like a well-formed cache. Underlying json parser threw "{}")",
            cacheLog, json->dump(), excep.what());

        return std::nullopt;
    }

    auto currentLastModified = lastModified();
    if (!currentLastModified)
    {
        spdlog::warn("{} Failed because there was an error while trying to retrieve source last modification time",
                     cacheLog);
        return std::nullopt;
    }

    if (*currentLastModified != mLastModified)
    {
        spdlog::debug(
            R"({} Failed because cache was modified, cache is dated: "{}", last source modification time "{}")",
            cacheLog, *mLastModified, *currentLastModified);

        return std::nullopt;
    }

    // Retrieving roms
    nlohmann::json romsArray;
    try
    {
        romsArray = json->at(ROMS_JSON_FIELD);
    }
    catch (const nlohmann::json::exception& excep)
    {
        spdlog::warn(
            R"({} Failed because "{}" does not look like a well-formed database. Underlying json parser threw "{}")",
            cacheLog, json->dump(), excep.what());

        return std::nullopt;
    }

    if (!romsArray.is_array())
    {
        spdlog::warn(R"({} Failed because "{}" does not look like a well-formed database. "{}" field is not an array")",
                     cacheLog, json->dump(), ROMS_JSON_FIELD);

        return std::nullopt;
    }

    for (const auto& rom : romsArray)
    {
        try
        {
            const auto& inserted = result.emplace_back(rom);
            spdlog::trace(R"({} rom found in cache: "{}")", cacheLog, inserted.toString());
        }
        catch (const nlohmann::json::exception& excep)
        {
            spdlog::warn(
                R"({} Cache entry "{}" does not look like a well formed rom, will not be added. Underlying json parser threw "{}")",
                cacheLog, rom.dump(), excep.what());
        }
    }

    spdlog::debug("{} Success. Cache had {} entries", cacheLog, result.size());
    return result;
}

std::vector<Rom::Game> Rom::Source::parse() const
{
    std::vector<Rom::Game> result;
    std::string scanLog("Rom parse operation.");
    auto files = scan();
    for (const auto& rom : files | std::ranges::views::filter(
                                       [](const std::filesystem::path& path) { return path.extension() == ".zip"; }))
    {
        auto info = romInfo(rom);

        if (!info)
        {
            spdlog::trace(R"({} File: "{}" does not look like a rom, will not be added)", scanLog, rom.string());
            continue;
        }

        // Searching for any screenshot associated to this rom
        std::optional<std::filesystem::path> screenshot;
        for (const auto& image : files | std::ranges::views::filter([](const std::filesystem::path& path) {
                                     return path.extension() == ".png" || path.extension() == ".jpeg" ||
                                            path.extension() == ".jpg";
                                 }))
        {
            if (image.stem() == rom.stem())
            {
                screenshot = image;
            }
        }

        result.emplace_back(rom, *info, Rom::Media{.screenshot{screenshot}});
    }

    return result;
}

std::optional<Rom::Info> Rom::Source::romInfo(const std::filesystem::path& path) const
{
    return Rom::Database::get().find(path.stem().string());
}

std::optional<nlohmann::json> Rom::Source::readCacheFile(const std::filesystem::path& path) const
{
    std::string cacheLog(fmt::format(R"(Cache read operation from "{}".)", path.string()));
    std::ifstream cacheFile(path.string());
    std::stringstream buffer;
    buffer << cacheFile.rdbuf();

    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(buffer.str());
    }
    catch (const nlohmann::json::exception& excep)
    {
        spdlog::debug(R"({} Failed because json could not be parsed, underlying json library threw "{}")", cacheLog,
                      excep.what());

        return std::nullopt;
    }

    spdlog::debug("{} Operation successful", cacheLog);
    return json;
}

bool Rom::Source::writeCache() const
{
    std::string writeLog(fmt::format(R"(Cache write operation to "{}".)", mCacheFile.string()));
    nlohmann::json json;

    if (!mMonitored)
    {
        spdlog::warn("{} Failed. Rom source was never monitored", writeLog);
        return false;
    }

    if (!mLastModified)
    {
        spdlog::warn("{} Failed. Could not retrieve last modification time", writeLog);
        return false;
    }

    json[LASTMODIFIED_JSON_FIELD] = *mLastModified;
    json[VERSION_JSON_FIELD] = version();

    nlohmann::json jsonRoms;
    auto roms = elements();
    for (const auto& rom : roms)
    {
        jsonRoms.push_back(rom);
    }

    json[ROMS_JSON_FIELD] = jsonRoms;

    if (!writeCacheFile(json, mCacheFile))
    {
        spdlog::warn("{} Failed. Could not write cache file", writeLog);
        return false;
    }

    spdlog::info("{} Successfully completed", writeLog);
    return true;
}

bool Rom::Source::writeCacheFile(const nlohmann::json& json, const std::filesystem::path& path) const
{
    // Nicely formatting our json
    std::stringstream stringStream;
    stringStream << std::setw(4) << json << std::endl;

    std::ofstream file(path.string().c_str());
    file << stringStream.str();

    return file.good();
}
