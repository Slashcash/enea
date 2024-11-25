#include "rom/game.hpp"

#include "spdlog/spdlog.h"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

static const std::filesystem::path ROM_PATH = std::filesystem::absolute("sf2.zip");

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static const bool ROM_IS_BIOS = false;

static const std::filesystem::path SCREENSHOT_PATH = std::filesystem::absolute("sf2.png");

static const Rom::Info INFO_COMPLETE{
    .title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};

static const Rom::Media MEDIA_COMPLETE{.screenshot = SCREENSHOT_PATH};

using namespace Rom;

/*
    We construct a game.
    Expectation: the game is correctly constructed.
*/
TEST(Game, construct)
{
    Rom::Game game(ROM_PATH, INFO_COMPLETE, MEDIA_COMPLETE);
    EXPECT_EQ(game.path(), ROM_PATH);
    EXPECT_EQ(game.info(), INFO_COMPLETE);
    EXPECT_TRUE(game.media() && *(game.media()) == MEDIA_COMPLETE);
}

/*
    We construct a game from a complete json.
    Expectation: the game is correctly constructed.
*/
TEST(Game, fromCompleteJson)
{
    auto game = Game{nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                                    {Game::INFO_JSON_FIELD, INFO_COMPLETE},
                                    {Game::MEDIA_JSON_FIELD, MEDIA_COMPLETE}}};

    EXPECT_EQ(game.path(), ROM_PATH);
    EXPECT_EQ(game.info(), INFO_COMPLETE);
    EXPECT_TRUE(game.media() && *(game.media()) == MEDIA_COMPLETE);
}

/*
    We construct a game from an empty json.
    Expectation: we throw.
*/
TEST(Game, fromEmptyJson)
{
    EXPECT_THROW((Game{nlohmann::json{}}), nlohmann::json::exception);
}

/*
    We construct a game from a json missing the rom path.
    Expectation: we throw.
*/
TEST(Game, fromJsonMissingPath)
{
    EXPECT_THROW(
        (Game{nlohmann::json{{Game::INFO_JSON_FIELD, INFO_COMPLETE}, {Game::MEDIA_JSON_FIELD, MEDIA_COMPLETE}}}),
        nlohmann::json::exception);
}

/*
    We construct a game from a json with an invalid path.
    Expectation: we throw.
*/
TEST(Game, fromJsonInvalidPath)
{
    EXPECT_THROW((Game{nlohmann::json{{Game::PATH_JSON_FIELD, 0},
                                      {Game::INFO_JSON_FIELD, INFO_COMPLETE},
                                      {Game::MEDIA_JSON_FIELD, MEDIA_COMPLETE}}}),
                 nlohmann::json::exception);
}

/*
    We construct a game from a json with missing info.
    Expectation: we throw.
*/
TEST(Game, fromJsonMissingInfo)
{
    EXPECT_THROW((Game{nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH}, {Game::MEDIA_JSON_FIELD, MEDIA_COMPLETE}}}),
                 nlohmann::json::exception);
}

/*
    We construct a game from a json with missing infos.
    Expectation: we throw.
*/
TEST(Game, fromJsonInvalidInfo)
{
    EXPECT_THROW(
        (Game{nlohmann::json{
            {Game::PATH_JSON_FIELD, ROM_PATH}, {Game::INFO_JSON_FIELD, 0}, {Game::MEDIA_JSON_FIELD, MEDIA_COMPLETE}}}),
        nlohmann::json::exception);
}

/*
    We construct a game from a json with missing medias.
    Expectation: the game is correctly constructed as this is an optional field.
*/
TEST(Game, fromJsonMissingMedia)
{
    auto game = Game{nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH}, {Game::INFO_JSON_FIELD, INFO_COMPLETE}}};

    EXPECT_EQ(game.path(), ROM_PATH);
    EXPECT_EQ(game.info(), INFO_COMPLETE);
    EXPECT_FALSE(game.media());
}

/*
    We construct a game from a json with invalid media.
    Expectation: we throw.
*/
TEST(Game, fromJsonInvalidMedia)
{
    EXPECT_THROW(
        (Game{nlohmann::json{
            {Game::PATH_JSON_FIELD, ROM_PATH}, {Game::INFO_JSON_FIELD, INFO_COMPLETE}, {Game::MEDIA_JSON_FIELD, 0}}}),
        nlohmann::json::exception);
}

/*
    We convert a complete game to a json.
    Expectation: the json is correctly constructed.
*/
TEST(Game, toJsonComplete)
{
    nlohmann::json json(Rom::Game(ROM_PATH, INFO_COMPLETE, MEDIA_COMPLETE));

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json.at(Game::INFO_JSON_FIELD).get<Rom::Info>() == INFO_COMPLETE);
    EXPECT_TRUE(json.contains(Game::MEDIA_JSON_FIELD) &&
                json.at(Game::MEDIA_JSON_FIELD).get<Rom::Media>() == MEDIA_COMPLETE);
}

/*
    We convert a game with missing media to a json.
    Expectation: the json is correctly constructed without the media.
*/
TEST(Game, toJsonMissingMedia)
{
    nlohmann::json json(Rom::Game(ROM_PATH, INFO_COMPLETE));

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json.at(Game::INFO_JSON_FIELD).get<Rom::Info>() == INFO_COMPLETE);
    EXPECT_FALSE(json.contains(Game::MEDIA_JSON_FIELD));
}

/*
    We compare two games.
    Expectation: they are equal if their path matches.
*/
TEST(Game, compare)
{
    // Even though their title don't match we considering matching paths as enough
    Rom::Game game1(ROM_PATH, INFO_COMPLETE);
    Rom::Game game2(ROM_PATH, Rom::Info{.title{"Metal Slug"}});

    EXPECT_TRUE(game1 == game2);
}

/*
    We convert a game to a string.
    Expectation: The rom title is returned.
*/
TEST(Game, toString)
{
    Rom::Game game(ROM_PATH, INFO_COMPLETE);
    EXPECT_EQ(game.toString(), ROM_TITLE);
}
