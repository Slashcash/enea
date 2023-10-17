#include "rom.hpp"

#include <cstdlib>
#include <exception>
#include <map>
#include <nlohmann/detail/iterators/iter_impl.hpp>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/json.hpp>
#include <sstream>

Rom::Rom(const std::filesystem::path& path) : mPath(path)
{
    checkPathValidity();
}

Rom::Rom(const nlohmann::json& j)
{
    from_json(j, *this);
    checkPathValidity();
}

void Rom::checkPathValidity() const
{
    if (mPath.is_relative())
    {
        throw Exception("Rom build with relative path");
    }

    if (!mPath.has_stem())
    {
        throw Exception("Rom path does not have stem");
    }
}

int Rom::runEmulator(const std::filesystem::path& romPath) const
{
    std::string advMameCmd = "advmame";
    std::string advMameRom = romPath.stem();

    std::stringstream cmd;
    cmd << advMameCmd << " --dir_rom " << romPath.parent_path() << " " << advMameRom;

    return std::system(cmd.str().c_str());
}

std::optional<std::error_code> Rom::fileExists(const std::filesystem::path& path) const
{
    if (std::error_code ec; !std::filesystem::is_regular_file(path, ec))
    {
        return ec;
    }

    return std::nullopt;
}

std::optional<Rom::Error> Rom::launch() const
{
    if (fileExists(mPath).has_value())
    {
        return Error::INVALID_ROM_FILE;
    }

    if (int ec = runEmulator(mPath); ec != 0)
    {
        return Error::EMULATOR_ERROR;
    }

    return std::nullopt;
}

void to_json(nlohmann::json& j, const Rom& rom)
{
    j.clear();
    j["path"] = rom.mPath;
    if (rom.mName.has_value())
    {
        j["name"] = rom.mName.value();
    }
}

void from_json(const nlohmann::json& j, Rom& rom)
{
    try
    {
        j.at("path").get_to(rom.mPath);
    }
    catch (const std::exception& e)
    {
        throw Rom::Exception("Rom constructed from invalid json");
    }

    if (auto it = j.find("name"); it != j.end())
    {
        try
        {
            rom.mName = *it;
        }
        catch (const std::exception& e)
        {
            rom.mName.reset();
        }
    }
}