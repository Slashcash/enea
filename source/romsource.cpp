#include "romsource.hpp"

#include "rom.hpp"

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

std::optional<RomSource::Error> RomSource::monitor() const
{
    if (auto err = folderExists(mPath); err.has_value())
    {
        return Error::INVALID_PATH;
    }

    for (auto paths = scanFolder(mPath); const auto& path : paths)
    {
        try
        {
            addRom(Rom(path));
        }
        catch ([[maybe_unused]] const Rom::Exception& e)
        {
            continue;
        }
    }

    return std::nullopt;
}