#include "configuration.hpp"

#include <cstdlib>

#include "softwareinfo.hpp"

std::optional<std::filesystem::path> Conf::homeDirectory() const
{
#ifdef TARGET_OS_LINUX
    if (auto* envValue = std::getenv("HOME"); envValue != nullptr)
    {
        return envValue;
    }

#elif defined(TARGET_OS_WINDOWS)
    auto* homeDrive = std::getenv("HOMEDRIVE");
    auto* homePath = std::getenv("HOMEPATH");
    if (homeDrive != nullptr && homePath != nullptr)
    {
        return std::string(homeDrive) + std::string(homePath);
    }

#else
#error "Unknown target OS. Compilation halted."
#endif

    return std::nullopt;
}

std::filesystem::path Conf::baseDirectory() const
{
    auto home = homeDirectory();
    if (!home)
    {
        throw Conf::Exception("HOME folder path cannot be retrieved");
    }

    return *home / ("." + std::string(executableName));
}

std::filesystem::path Conf::romDirectory() const
{
    return baseDirectory() / "roms";
}

std::filesystem::path Conf::cacheDirectory() const
{
    return baseDirectory() / "cache";
}

std::filesystem::path Conf::advMameConfigurationFile() const
{
    return baseDirectory() / "advmame.rc";
}
