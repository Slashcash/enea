#include "romdb.hpp"

#include <cstring>
#include <iterator>
#include <system_error>
#include <type_traits>

#include <cmrc/cmrc.hpp>

namespace cmrc {
namespace detail {
struct dummy;
}
} // namespace cmrc
CMRC_DECLARE(resources);

std::pair<std::optional<RomDB::Error>, std::string> RomDB::loadFromFile(const std::filesystem::path& path) const
{
    auto embeddedFS = cmrc::resources::get_filesystem();
    cmrc::file dbFile;
    try
    {
        dbFile = embeddedFS.open(path);
    }
    catch ([[maybe_unused]] const std::system_error& exception)
    {
        return std::make_pair(Error::UNABLE_TO_LOAD_DB_FILE, std::string());
    }

    return std::make_pair(std::nullopt, std::string(dbFile.begin(), std::distance(dbFile.begin(), dbFile.end())));
}

std::optional<RomDB::Error> RomDB::load()
{
    // No need to reload if already loaded
    if (mLoaded)
    {
        return std::nullopt;
    }

    // Loading from file
    if (auto [error, xml] = loadFromFile(mPath); error.has_value())
    {
        return error;
    }

    else
    {
        if (mXML.Parse(xml.c_str()) != tinyxml2::XML_SUCCESS)
        {
            return Error::UNABLE_TO_PARSE_DB_FILE;
        }
    }

    mRootElement = mXML.RootElement()->FirstChild()->NextSiblingElement("game");
    mLoaded = true;
    return std::nullopt;
}

tinyxml2::XMLElement* RomDB::findRom(const std::string& rom) const
{
    auto roms = mRootElement;
    while (roms != nullptr && std::strcmp(roms->Attribute("name"), rom.c_str()) != 0)
    {
        roms = roms->NextSiblingElement("game");
    }

    return roms;
}

std::optional<RomDB::RomInfo> RomDB::find(const std::string& rom) const
{
    // Return an empty result if the db was not loaded
    if (!mLoaded)
    {
        return std::nullopt;
    }

    auto info = findRom(rom);

    // Return an empty result if we didn't find the rom
    if (info == nullptr)
    {
        return std::nullopt;
    }

    // Finding the rom name
    RomInfo result;
    if (auto name = info->FirstChildElement("description"); name != nullptr)
    {
        result.name = name->GetText();
    }

    // Finding the rom year
    if (auto year = info->FirstChildElement("year"); year != nullptr)
    {
        result.year = year->GetText();
    }

    // Finding the rom manufacturer
    // Finding the rom year
    if (auto manufacturer = info->FirstChildElement("manufacturer"); manufacturer != nullptr)
    {
        result.manufacturer = manufacturer->GetText();
    }

    return result;
}