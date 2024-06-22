#include "rom.hpp"

Rom::Rom(std::filesystem::path path) : mPath(std::move(path))
{
}

Rom::Rom(std::filesystem::path path, RomInfo info) : mPath(std::move(path)), mInfo(std::move(info))
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

std::optional<bool> Rom::isBios() const
{
    return mInfo.isBios;
}

void Rom::setMedia(const RomMedia& media)
{
    mMedia = media;
}

RomInfo Rom::info() const
{
    return mInfo;
}

RomMedia Rom::media() const
{
    return mMedia;
}

std::optional<std::filesystem::path> Rom::screenshot() const
{
    return mMedia.screenshot;
}

bool Rom::operator==(const Rom& rom) const
{
    return this->mPath == rom.mPath;
}
