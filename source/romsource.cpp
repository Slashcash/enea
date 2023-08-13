#include "romsource.hpp"

#include "rom.hpp"

RomSource::RomSource(const std::filesystem::path& path) : mPath(path)
{
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

std::filesystem::path RomSource::path() const
{
    return mPath;
}

std::variant<std::vector<Rom>, RomSource::Error> RomSource::scan() const
{
    std::vector<Rom> result;

    if (folderExists(mPath).has_value())
    {
        return Error::DIRECTORY_NOT_EXISTING;
    }

    auto paths = scanFolder(mPath);
    for (const auto path : paths)
    {
        std::error_code ec;
        auto absolutePath = std::filesystem::absolute(path);

        if (!ec)
        {
            result.emplace_back(Rom(absolutePath));
        }
    }

    return result;
}