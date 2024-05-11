#include "configuration.hpp"

#include <cstdlib>

#include "softwareinfo.hpp"

std::optional<std::filesystem::path> Configuration::homeDirectory() const
{
    if (auto* envValue = std::getenv("HOME"); envValue != nullptr)
    {
        return envValue;
    }

    return std::nullopt;
}

std::filesystem::path Configuration::baseDirectory() const
{
    auto home = homeDirectory();
    if (!home)
    {
        throw Configuration::Excep("HOME folder path cannot be retrieved");
    }

    return *home / ("." + std::string(executableName));
}

std::filesystem::path Configuration::romDirectory() const
{
    return baseDirectory() / "roms";
}

std::filesystem::path Configuration::cacheFile() const
{
    return baseDirectory() / "cache.json";
}
