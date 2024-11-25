#include "rom/game.hpp"

Rom::Game::Game(const std::filesystem::path& path, const Rom::Info& info, const std::optional<Rom::Media>& media)
    : mPath(path), mInfo(info), mMedia(media)
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

std::optional<Rom::Media> Rom::Game::media() const
{
    return mMedia;
}

bool Rom::Game::operator==(const Game& rom) const
{
    return this->mPath == rom.mPath;
}

std::string Rom::Game::toString() const
{
    return info().title;
}
