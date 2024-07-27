#include "configuration.hpp"

#include <cstdlib>

#include "softwareinfo.hpp"

std::optional<std::filesystem::path> Conf::homeDirectory() const
{
    if (auto* envValue = std::getenv("HOME"); envValue != nullptr)
    {
        return envValue;
    }

    return std::nullopt;
}

std::filesystem::path Conf::baseDirectory() const
{
    auto home = homeDirectory();
    if (!home)
    {
        throw Conf::Excep("HOME folder path cannot be retrieved");
    }

    return *home / ("." + std::string(executableName));
}

std::filesystem::path Conf::romDirectory() const
{
    return baseDirectory() / "roms";
}

std::filesystem::path Conf::cacheFile() const
{
    return baseDirectory() / "cache.json";
}
