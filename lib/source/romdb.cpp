#include "romdb.hpp"

#include <cstring>
#include <iterator>
#include <system_error>

#include <cmrc/cmrc.hpp>

// NOLINTBEGIN
namespace cmrc {
namespace detail {
struct dummy;
}
} // namespace cmrc
// NOLINTEND

CMRC_DECLARE(resources);

nlohmann::json RomDB::loadFromFile() const
{
    // We are not really loading rom database from filesystem, it's really embedded into our executable's filesystem
    auto embeddedFS = cmrc::resources::get_filesystem();
    cmrc::file dbFile;
    try
    {
        dbFile = embeddedFS.open("romdb/romdb.json");
    }
    catch (...)
    {
        throw RomDB::Excep("Unable to load rom database");
    }

    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(
            std::string{dbFile.begin(), static_cast<std::size_t>(std::distance(dbFile.begin(), dbFile.end()))});
    }
    catch (...)
    {
        throw RomDB::Excep("Unable to parse rom database from json");
    }

    return json;
}

bool RomDB::loaded() const
{
    return mLoaded;
}

void RomDB::load()
{
    // No need to reload if already loaded
    if (mLoaded)
    {
        return;
    }

    auto json = loadFromFile();

    // If the db does not contain the "roms" or is not an array element we deem it invalid
    if (!json.contains("roms") || !json["roms"].is_array())
    {
        throw RomDB::Excep("Invalid rom database");
    }

    auto roms = json["roms"];

    for (const auto& rom : roms)
    {
        // If not enough information we do not add the rom to our database
        if (!rom.contains("name") || !rom.contains("info"))
        {
            continue;
        }

        mRecords.try_emplace(rom["name"], rom["info"]);
    }

    mLoaded = true;
}

std::optional<RomInfo> RomDB::find(const std::filesystem::path& rom) const
{
    // Return an empty result if the db was not loaded
    if (!mLoaded)
    {
        return std::nullopt;
    }

    // Return an empty result if the path has no stem
    if (!rom.has_stem())
    {
        return std::nullopt;
    }

    auto result = mRecords.find(rom.stem().string());

    // Return an empty result if we didn't find the rom
    if (result == mRecords.end())
    {
        return std::nullopt;
    }

    return result->second;
}
