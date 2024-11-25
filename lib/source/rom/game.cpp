#include "rom/game.hpp"

Rom::Game::Game(std::filesystem::path path) : mPath(std::move(path))
{
}

Rom::Game::Game(std::filesystem::path path, Rom::Info info, Rom::Media media)
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

Rom::Media Rom::Game::media() const
{
    return mMedia;
}

bool Rom::Game::operator==(const Game& rom) const
{
    return this->mPath == rom.mPath;
}
