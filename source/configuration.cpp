#include "configuration.hpp"

#include <cstdlib>
#include <system_error>

std::optional<std::filesystem::path> Configuration::homeDirectory() const
{
    if (auto envValue = std::getenv("HOME"); envValue != nullptr)
    {
        return envValue;
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> Configuration::tempHomeDirectory() const
{
    std::error_code ec;
    if (auto tempPath = std::filesystem::temp_directory_path(ec); !ec)
    {
        return tempPath;
    }

    return std::nullopt;
}

std::filesystem::path Configuration::baseDirectory() const
{
    if (auto home = homeDirectory(); home.has_value())
    {
        return home.value() / ".enea";
    }

    if (auto home = tempHomeDirectory(); home.has_value())
    {
        return home.value() / ".enea";
    }

    throw Configuration::Exception("Both HOME folder and TEMP folder are unsuitable as base directories");
}

std::filesystem::path Configuration::romDirectory() const
{
    return baseDirectory() / "roms";
}