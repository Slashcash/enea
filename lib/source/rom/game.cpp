#include "rom/game.hpp"

Rom::Game::Game(std::filesystem::path path) : mPath(std::move(path))
{
}

Rom::Game::Game(std::filesystem::path path, RomInfo info, RomMedia media)
    : mPath(std::move(path)), mInfo(std::move(info)), mMedia(std::move(media))
{
}

std::filesystem::path Rom::Game::path() const
{
    return mPath;
}

RomInfo Rom::Game::info() const
{
    return mInfo;
}

std::optional<std::string> Rom::Game::title() const
{
    return mInfo.title;
}

std::optional<std::string> Rom::Game::year() const
{
    return mInfo.year;
}

[[nodiscard]] std::optional<std::string> Rom::Game::manufacturer() const
{
    return mInfo.manufacturer;
}

std::optional<bool> Rom::Game::isBios() const
{
    return mInfo.isBios;
}

RomMedia Rom::Game::media() const
{
    return mMedia;
}

std::optional<std::filesystem::path> Rom::Game::screenshot() const
{
    return mMedia.screenshot;
}

bool Rom::Game::operator==(const Game& rom) const
{
    return this->mPath == rom.mPath;
}
