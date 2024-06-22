#include "romsource.hpp"

#include <algorithm>

RomSource::RomSource(std::filesystem::path romFolder) : mRomFolder(std::move(romFolder))
{
}

bool RomSource::fileIsImage(const std::filesystem::path& path)
{
    return path.extension() == ".png" || path.extension() == ".jpeg" || path.extension() == ".jpg";
}

bool RomSource::fileIsRom(const std::filesystem::path& path)
{
    return path.extension() == ".zip";
}

bool RomSource::isRomFolderValid() const
{
    return std::filesystem::is_directory(mRomFolder);
}

std::filesystem::path RomSource::romFolder() const
{
    return mRomFolder;
}

RomSource::ScanResult RomSource::scan() const
{
    ScanResult result;

    if (!isRomFolderValid())
    {
        throw Excep("Rom source folder is not a directory");
    }

    for (auto files = scanFolder(mRomFolder); const auto& file : files)
    {
        if (fileIsRom(file))
        {
            result.roms.push_back(file);
        }
        else if (fileIsImage(file))
        {
            result.screenshots.push_back(file);
        }
    }

    if (auto lastModified = lastFolderModification(mRomFolder); lastModified)
    {
        result.lastModified = *lastModified;
    }
    else
    {
        throw Excep("Can't read rom source last edit time");
    }

    return result;
}

std::list<std::filesystem::path> RomSource::scanFolder(const std::filesystem::path& path) const
{
    std::list<std::filesystem::path> result;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            result.push_back(std::filesystem::absolute(entry.path()));
        }
    }

    return result;
}

std::optional<std::string> RomSource::lastFolderModification(const std::filesystem::path& path) const
{
    std::error_code errorCode;

    // With this algorithm we scan all the subfolders of a folder and we find the most recently modified one, this way
    // we know which is the very last modification moment of a folder structure
    std::list<std::filesystem::file_time_type> editTimes;

    // First checking the modification time of the actual folder
    std::filesystem::file_time_type lastModified = std::filesystem::last_write_time(path, errorCode);
    if (!errorCode)
    {
        editTimes.push_back(lastModified);
    }

    // Then we scan all the subfolders for edit times
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
    {
        if (lastModified = std::filesystem::last_write_time(entry.path(), errorCode);
            entry.is_directory() && !errorCode)
        {
            editTimes.push_back(lastModified);
        }
    }

    if (editTimes.empty())
    {
        return std::nullopt;
    }

    // Then we find the most recent modification
    lastModified = *(std::max_element(editTimes.begin(), editTimes.end()));

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

std::string RomSource::lastEditTime() const
{
    if (auto editTime = lastFolderModification(mRomFolder); editTime)
    {
        return *editTime;
    }

    throw Excep("Can't read rom source last edit time");
}
