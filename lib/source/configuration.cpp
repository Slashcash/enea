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

std::optional<std::filesystem::path> Conf::executableDirectory() const
{
#ifdef TARGET_OS_LINUX
    std::error_code ec;
    if (auto execPath = std::filesystem::read_symlink("/proc/self/exe"); !ec && execPath.has_parent_path())
    {
        return execPath.parent_path();
    }

#elif defined(TARGET_OS_WINDOWS)
    // We need a good way to check executable path on Windows.
    return "";
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

std::filesystem::path Conf::bundledRomDirectory() const
{
    auto execPath = executableDirectory();
    if (!execPath)
    {
        throw Conf::Exception("Executable path cannot be retrieved");
    }

    return *execPath / ".." / "share" / std::string(executableName) / "bundled_roms";
}

std::filesystem::path Conf::cacheDirectory() const
{
    return baseDirectory() / "cache";
}

std::filesystem::path Conf::advMameConfigurationFile() const
{
    return baseDirectory() / "advmame.rc";
}

Conf::RenderMode Conf::availableRenderMode() const
{
#ifdef USE_DIRECT_RENDERING
    return RenderMode::DIRECT;
#else
    return RenderMode::WINDOW;
#endif
}
