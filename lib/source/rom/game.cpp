#include "rom/game.hpp"

Rom::Game::Game(std::filesystem::path path) : mPath(std::move(path))
{
}

Rom::Game::Game(std::filesystem::path path, Rom::Info info, RomMedia media)
    : mPath(std::move(path)), mInfo(std::move(info)), mMedia(std::move(media))
{
}

std::filesystem::path Rom::Game::path() const
{
    return mPath;
}

Rom::Info Rom::Game::info() const
{
    return mInfo;
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
