#include "rom.hpp"

Rom::Rom(const std::filesystem::path& path) : mPath(path)
{
}

std::optional<std::error_code> Rom::fileExists(const std::filesystem::path& path) const
{
    if (std::error_code ec; !std::filesystem::is_regular_file(path, ec))
    {
        return ec;
    }

    return std::nullopt;
}

int Rom::runEmulator(const std::filesystem::path& romPath) const
{
    return 0;
}

std::filesystem::path Rom::path() const
{
    return mPath;
}

std::string Rom::fileName() const
{
    return mPath.filename();
}

std::string Rom::name() const
{
    return mPath.stem();
}

std::optional<Rom::Error> Rom::launch() const
{
    using enum Rom::Error;
    if (!mPath.is_absolute())
    {
        return PATH_NOT_ABSOLUTE;
    }

    if (fileExists(mPath).has_value())
    {
        return ROM_FILE_NOT_EXISTING;
    }

    if (int ec = runEmulator(mPath); ec != 0)
    {
        return EMULATOR_ERROR;
    }

    return std::nullopt;
}