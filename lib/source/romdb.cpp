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

std::string RomDB::loadFromFile() const
{
    // We are not really loading rom database from filesystem, it's really embedded into our executable's filesystem
    auto embeddedFS = cmrc::resources::get_filesystem();
    cmrc::file dbFile;
    try
    {
        dbFile = embeddedFS.open("romdb/romdb.xml");
    }
    catch ([[maybe_unused]] const std::system_error& exception)
    {
        return {};
    }

    return {dbFile.begin(), static_cast<std::size_t>(std::distance(dbFile.begin(), dbFile.end()))};
}

bool RomDB::loaded() const
{
    return mLoaded;
}

std::optional<RomDB::Error> RomDB::load()
{
    // No need to reload if already loaded
    if (mLoaded)
    {
        return std::nullopt;
    }

    // Parsing string into an xml
    if (mXML.Parse(loadFromFile().c_str()) != tinyxml2::XML_SUCCESS)
    {
        return Error::UNABLE_TO_PARSE_DB_FILE;
    }

    mRootElement = mXML.RootElement()->FirstChild()->NextSiblingElement("game");

    if (mRootElement == nullptr)
    {
        return Error::UNABLE_TO_PARSE_DB_FILE;
    }

    mLoaded = true;
    return std::nullopt;
}

tinyxml2::XMLElement* RomDB::findRom(const std::string& rom) const
{
    auto* roms = mRootElement;
    while (roms != nullptr && roms->Attribute("name") != nullptr &&
           std::strcmp(roms->Attribute("name"), rom.c_str()) != 0)
    {
        roms = roms->NextSiblingElement("game");
    }

    return roms;
}

std::optional<RomDB::RomInfo> RomDB::find(const std::filesystem::path& rom) const
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

    auto* info = findRom(rom.stem());

    // Return an empty result if we didn't find the rom
    if (info == nullptr)
    {
        return std::nullopt;
    }

    // Finding the rom name
    RomInfo result;
    if (auto* title = info->FirstChildElement("description"); title != nullptr)
    {
        result.title = title->GetText();
    }

    // Finding the rom year
    if (auto* year = info->FirstChildElement("year"); year != nullptr)
    {
        result.year = year->GetText();
    }

    // Finding the rom manufacturer
    if (auto* manufacturer = info->FirstChildElement("manufacturer"); manufacturer != nullptr)
    {
        result.manufacturer = manufacturer->GetText();
    }

    // Finding if it is a bios
    if (const auto* bios = info->Attribute("isbios"); bios != nullptr && std::strcmp(bios, "yes") == 0)
    {
        result.isBios = true;
    }
    else
    {
        result.isBios = false;
    }

    return result;
}
