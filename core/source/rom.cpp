#include "rom.hpp"

Rom::Rom(const std::filesystem::path& path) : mPath(path)
{
}

Rom::Rom(const std::filesystem::path& path, const RomDB::RomInfo& info) : mPath(path), mInfo(info)
{
}

std::filesystem::path Rom::path() const
{
    return mPath;
}

std::optional<std::string> Rom::title() const
{
    return mInfo.title;
}

std::optional<std::string> Rom::year() const
{
    return mInfo.year;
}

[[nodiscard]] std::optional<std::string> Rom::manufacturer() const
{
    return mInfo.manufacturer;
}

bool Rom::operator==(const Rom& rom) const
{
    return this->mPath == rom.mPath;
}