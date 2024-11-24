#include "rom/game.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

static const std::filesystem::path ROM_PATH = std::filesystem::absolute("sf2.zip");

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static const bool ROM_IS_BIOS = false;

static const std::filesystem::path SCREENSHOT_PATH = std::filesystem::absolute("sf2.png");

static const Rom::Info INFO_SET_COMPLETE{
    .title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};
static const Rom::Info INFO_SET_TITLE_MISSING{
    .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};
static const Rom::Info INFO_SET_YEAR_MISSING{
    .title = ROM_TITLE, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};
static const Rom::Info INFO_SET_MANUFACTURER_MISSING{.title = ROM_TITLE, .year = ROM_YEAR, .isBios = ROM_IS_BIOS};
static const Rom::Info INFO_SET_BIOS_MISSING{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER};
static const Rom::Info INFO_SET_EMPTY{};

static const RomMedia MEDIA_SET_COMPLETE{.screenshot = SCREENSHOT_PATH};
static const RomMedia MEDIA_SET_SCREENSHOT_MISSING{};
static const RomMedia MEDIA_SET_EMPTY{};

using namespace Rom;

TEST(Game, fromJson)
{
    /*
        Building Game from a json which contains all the information.
        Expectation: Game is correctly built and every field is correctly set.
    */
    auto rom = Game{nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                                   {Game::INFO_JSON_FIELD, INFO_SET_COMPLETE},
                                   {Game::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_EQ(rom.info(), INFO_SET_COMPLETE);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a json which only contains the rom path.
        Expectation: Game is correctly built but contains no additional information except from path.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_EQ(rom.info(), INFO_SET_EMPTY);
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Game from a json which does not contain a path.
        Expectation: Construction will throw since this should not be allowed.
    */
    EXPECT_THROW((Game{nlohmann::json{{Game::INFO_JSON_FIELD, INFO_SET_COMPLETE},
                                      {Game::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}}}),
                 Game::Exception);

    /*
        Building Game from a json which contains all the information except for the rom title.
        Expectation: Game is correctly built and every field is correctly set apart from the rom title.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                         {Game::INFO_JSON_FIELD, INFO_SET_TITLE_MISSING},
                         {Game::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_EQ(rom.info(), INFO_SET_TITLE_MISSING);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a json which contains all the information except for the rom year.
        Expectation: Game is correctly built and every field is correctly set apart from the rom year.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                         {Game::INFO_JSON_FIELD, INFO_SET_YEAR_MISSING},
                         {Game::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_EQ(rom.info(), INFO_SET_YEAR_MISSING);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a json which contains all the information except for the rom manufacturer.
        Expectation: Game is correctly built and every field is correctly set apart from the rom manufacturer.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                         {Game::INFO_JSON_FIELD, INFO_SET_MANUFACTURER_MISSING},
                         {Game::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_EQ(rom.info(), INFO_SET_MANUFACTURER_MISSING);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a json which contains all the information except for the rom bios.
        Expectation: Game is correctly built and every field is correctly set apart from the rom bios.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                         {Game::INFO_JSON_FIELD, INFO_SET_BIOS_MISSING},
                         {Game::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a json which contains no information.
        Expectation: Game is correctly built and no information is set.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                         {Game::INFO_JSON_FIELD, INFO_SET_EMPTY},
                         {Game::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a json which contains all the information except for the screenshot.
        Expectation: Game is correctly built and every field is correctly set apart from the screenshot.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                         {Game::INFO_JSON_FIELD, INFO_SET_COMPLETE},
                         {Game::MEDIA_JSON_FIELD, MEDIA_SET_SCREENSHOT_MISSING}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Game from a json which contains no media information.
        Expectation: Game is correctly built and no media is set.
    */
    rom = nlohmann::json{{Game::PATH_JSON_FIELD, ROM_PATH},
                         {Game::INFO_JSON_FIELD, INFO_SET_COMPLETE},
                         {Game::MEDIA_JSON_FIELD, MEDIA_SET_EMPTY}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.screenshot());
}

TEST(Game, toJson)
{
    /*
        Building a JSON from a rom that contains all the information.
        Expectation: json contains all the information.
    */
    nlohmann::json json = Game{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::TITLE_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Game::MEDIA_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that only contains a path.
        Expectation: json only contains info about the path.
    */
    json = Game{ROM_PATH};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD));
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD));
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD));
    EXPECT_FALSE(json.contains(Game::MEDIA_JSON_FIELD) &&
                 json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD));

    /*
        Building a JSON from a rom that contains all the information except for the rom title.
        Expectation: json contains all the information except for the rom title.
    */
    json = Game{ROM_PATH, INFO_SET_TITLE_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Game::MEDIA_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except the rom year.
        Expectation: json contains all the information except for the rom year.
    */
    json = Game{ROM_PATH, INFO_SET_YEAR_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::TITLE_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD));
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Game::MEDIA_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except the rom manufacturer.
        Expectation: json contains all the information except for the rom manufacturer.
    */
    json = Game{ROM_PATH, INFO_SET_MANUFACTURER_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::TITLE_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD));
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Game::MEDIA_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except for the bios.
        Expectation: json contains all the information except for the bios.
    */
    json = Game{ROM_PATH, INFO_SET_BIOS_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::TITLE_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD));
    EXPECT_TRUE(json.contains(Game::MEDIA_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains no rom info.
        Expectation: json contains no rom info.
    */
    json = Game{ROM_PATH, INFO_SET_EMPTY, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD));
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD));
    EXPECT_FALSE(json.contains(Game::INFO_JSON_FIELD) &&
                 json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD));
    EXPECT_TRUE(json.contains(Game::MEDIA_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Game::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except the screenshot.
        Expectation: json contains all the information except for the screenshot.
    */
    json = Game{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_SCREENSHOT_MISSING};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::TITLE_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_FALSE(json.contains(Game::MEDIA_JSON_FIELD) &&
                 json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD));

    /*
        Building a JSON from a rom that contains no rom media.
        Expectation: json contains no rom media.
    */
    json = Game{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_EMPTY};

    EXPECT_TRUE(json.contains(Game::PATH_JSON_FIELD) && json[Game::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::TITLE_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::YEAR_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Game::INFO_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD].contains(Rom::Info::ISBIOS_JSON_FIELD) &&
                json[Game::INFO_JSON_FIELD][Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_FALSE(json.contains(Game::MEDIA_JSON_FIELD) &&
                 json[Game::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD));
}

TEST(Game, fromConstructor)
{
    /*
        Building Game from a constructor which contains all the information.
        Expectation: Game is correctly built and every field is correctly set.
    */
    Game rom{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a constructor which only contains the rom path.
        Expectation: Game is correctly built but contains no additional information except from path.
    */
    rom = Game{ROM_PATH};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Game from a constructor which contains all the information except for the rom title.
        Expectation: Game is correctly built and every field is correctly set apart from the rom title.
    */
    rom = Game{ROM_PATH, INFO_SET_TITLE_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a constructor which contains all the information except for the rom year.
        Expectation: Game is correctly built and every field is correctly set apart from the rom year.
    */
    rom = Game{ROM_PATH, INFO_SET_YEAR_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a json constructor contains all the information except for the rom manufacturer.
        Expectation: Game is correctly built and every field is correctly set apart from the rom manufacturer.
    */
    rom = Game{ROM_PATH, INFO_SET_MANUFACTURER_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a constructor which contains all the information except for the rom bios.
        Expectation: Game is correctly built and every field is correctly set apart from the rom bios.
    */
    rom = Game{ROM_PATH, INFO_SET_BIOS_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a constructor which contains no information.
        Expectation: Game is correctly built and no information is set.
    */
    rom = Game{ROM_PATH, INFO_SET_EMPTY, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Game from a constructor which contains all the information except for the screenshot.
        Expectation: Game is correctly built and every field is correctly set apart from the screenshot.
    */
    rom = Game{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_SCREENSHOT_MISSING};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Game from a constructor which contains no media information.
        Expectation: Game is correctly built and no media is set.
    */
    rom = Game{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_EMPTY};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.screenshot());
}
