#include "rom/folder.hpp"

#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <spdlog/spdlog.h>

std::vector<std::filesystem::path> Rom::Folder::scan() const
{
    std::vector<std::filesystem::path> result;
    std::string listLog(fmt::format(R"(Rom scan operation on folder "{}".)", mFolderPath.string()));
    auto entries = fileList(mFolderPath);
    for (const auto& entry : entries)
    {
        if (std::filesystem::is_regular_file(entry))
        {
            auto path = std::filesystem::absolute(entry);
            result.push_back(path);
            spdlog::trace(R"({} Found file: "{}")", listLog, path.string());
        }
    }

    spdlog::debug(R"({} Successful. Found {} file")", listLog, result.size());
    return result;
}

std::vector<std::filesystem::path> Rom::Folder::fileList(const std::filesystem::path& folder) const
{
    /* We provide two different implementations to iterate over the content of a folder:
        1. The first one uses POSIX API
        2. The second one uses std::filesystem::recursive_directory_iterator

        For some reasons our toolchain adds some strange symbols when using recursive_directory_iterator.
        These symbols are generally unavailable on raspbian (?) and this makes the executable unusable.
        For this reason we use the POSIX API when compiling arm so we don't break raspbian compatibility.
        This issue should be investigated.
    */
    std::vector<std::filesystem::path> result;

#ifdef USE_POSIX_FILE_LIST
    DIR* dp;
    struct dirent* entry;
    struct stat info;

    if ((dp = opendir(folder.c_str())) == NULL)
    {
        return result;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        std::filesystem::path path = folder / entry->d_name;

        if (stat(path.c_str(), &info) != 0)
        {
            continue;
        }

        if (S_ISDIR(info.st_mode))
        {
            // Skip "." and ".." directories
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                // Recurse into subdirectories
                std::vector<std::filesystem::path> subDirFiles = fileList(path);
                result.insert(result.end(), subDirFiles.begin(), subDirFiles.end());
            }
        }
        else
        {
            // It's a file, add to list
            result.push_back(path);
        }
    }

    closedir(dp);
#else
    try
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(folder))
        {
            result.push_back(std::filesystem::absolute(entry.path()));
        }
    }
    catch (const std::filesystem::filesystem_error&)
    {
        return std::vector<std::filesystem::path>();
    }
#endif

    return result;
}

std::optional<std::string> Rom::Folder::lastModified() const
{
    std::error_code errorCode;

    // With this algorithm we scan all the subfolders of a folder and we find the most recently modified one, this
    // way we know which is the very last modification moment of a folder structure
    std::vector<std::filesystem::file_time_type> editTimes;

    // First checking the modification time of the actual folder
    std::filesystem::file_time_type lastModified = std::filesystem::last_write_time(mFolderPath, errorCode);
    if (!errorCode)
    {
        editTimes.push_back(lastModified);
    }

    // Then we scan all the subfolders for edit times
    auto entries = fileList(mFolderPath);
    for (const auto& entry : entries)
    {
        if (lastModified = std::filesystem::last_write_time(entry, errorCode);
            std::filesystem::is_directory(entry) && !errorCode)
        {
            editTimes.push_back(lastModified);
        }
    }

    if (editTimes.empty())
    {
        return std::nullopt;
    }

    // Then we find the most recent modification
    lastModified = *(std::ranges::max_element(editTimes));

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
