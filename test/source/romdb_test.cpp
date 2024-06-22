#include "romdb_mock.hpp"

#include <gtest/gtest.h>

static const std::filesystem::path ROM_PATH = std::filesystem::absolute("sf2.zip");
static const std::filesystem::path ROM_PATH_NOTITLE = std::filesystem::absolute("ghouls.zip");
static const std::filesystem::path ROM_PATH_NOYEAR = std::filesystem::absolute("mslug.zip");
static const std::filesystem::path ROM_PATH_NOMANUFACTURER = std::filesystem::absolute("tetris.zip");
static const std::filesystem::path ROM_PATH_NOBIOS = std::filesystem::absolute("mslug2.zip");
static const std::filesystem::path ROM_PATH_NONAME = std::filesystem::absolute("avsp.zip");
static const std::filesystem::path ROM_PATH_NOINFO = std::filesystem::absolute("sfa3.zip");

static const std::string ROM_TITLE = "Street Fighter II - The World Warrior (World 910522)";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static const bool ROM_ISBIOS = false;

TEST(RomDB, construct)
{
    /*
        Constructing with default constructor.
        Expectation: the rom database will be not loaded and any query will return an empty result.
    */
    RomDBMock romDB;
    EXPECT_FALSE(romDB.loaded());
    EXPECT_FALSE(romDB.find(ROM_PATH));
}

TEST(RomDB, load)
{
    /*
        Loading a json file with no roms.
        Expectation: loading will throw as this is an invalid file.
    */
    RomDBMock romDB;
    EXPECT_CALL(romDB, loadFromFile()).Times(1).WillOnce(testing::Return(nlohmann::json{{"error", "ignored"}}));
    EXPECT_THROW(romDB.load(), RomDB::Excep);
    EXPECT_FALSE(romDB.loaded());
    EXPECT_FALSE(romDB.find(ROM_PATH));

    /*
        Loading a json file with a roms field which is not an array.
        Expectation: loading will throw as this is an invalid file
    */
    EXPECT_CALL(romDB, loadFromFile()).Times(1).WillOnce(testing::Return(nlohmann::json{{"roms", "ignored"}}));
    EXPECT_THROW(romDB.load(), RomDB::Excep);
    EXPECT_FALSE(romDB.loaded());
    EXPECT_FALSE(romDB.find(ROM_PATH));

    /*
        Loading a correct json database file.
        Expectation: loading will not throw, database will be loaded.
    */

    // clang-format off
   EXPECT_CALL(romDB, loadFromFile()).Times(1).WillOnce(testing::Return(nlohmann::json{{
    "roms", {
    {{"name", ROM_PATH.stem().string()}, {"info", RomInfo{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_ISBIOS}}},
    {{"name", ROM_PATH_NOTITLE.stem().string()}, {"info", RomInfo{.year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_ISBIOS}}},
    {{"name", ROM_PATH_NOYEAR.stem().string()}, {"info", RomInfo{.title = ROM_TITLE, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_ISBIOS}}},
    {{"name", ROM_PATH_NOMANUFACTURER.stem().string()}, {"info", RomInfo{.title = ROM_TITLE, .year = ROM_YEAR, .isBios = ROM_ISBIOS}}},
    {{"name", ROM_PATH_NOBIOS.stem().string()}, {"info", RomInfo{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER}}},
    {{"info", RomInfo{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER}}},
    {{"name", ROM_PATH_NOINFO.stem().string()}}
    }
    }}));
    // clang-format on

    EXPECT_NO_THROW(romDB.load());
    EXPECT_TRUE(romDB.loaded());

    /*
        Loading a rom database that was already loaded.
        Expectation: operation should succeed without physically loading the file again.
    */
    EXPECT_CALL(romDB, loadFromFile()).Times(0);
    EXPECT_NO_THROW(romDB.load());
    EXPECT_TRUE(romDB.loaded());

    /*
        Performing queries on a correctly loaded database.
        Expectation: all the queries will succeed and return coherent results.
    */

    // This rom should have complete information
    auto rom = romDB.find(ROM_PATH);
    EXPECT_TRUE(rom && rom->title && *(rom->title) == ROM_TITLE);
    EXPECT_TRUE(rom && rom->year && *(rom->year) == ROM_YEAR);
    EXPECT_TRUE(rom && rom->manufacturer && *(rom->manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom && rom->isBios && *(rom->isBios) == ROM_ISBIOS);

    // This rom should miss the title
    rom = romDB.find(ROM_PATH_NOTITLE);
    EXPECT_FALSE(rom && rom->title);
    EXPECT_TRUE(rom && rom->year && *(rom->year) == ROM_YEAR);
    EXPECT_TRUE(rom && rom->manufacturer && *(rom->manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom && rom->isBios && *(rom->isBios) == ROM_ISBIOS);

    // This rom should miss the year
    rom = romDB.find(ROM_PATH_NOYEAR);
    EXPECT_TRUE(rom && rom->title && *(rom->title) == ROM_TITLE);
    EXPECT_FALSE(rom && rom->year);
    EXPECT_TRUE(rom && rom->manufacturer && *(rom->manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom && rom->isBios && *(rom->isBios) == ROM_ISBIOS);

    // This rom should miss the manufacturer
    rom = romDB.find(ROM_PATH_NOMANUFACTURER);
    EXPECT_TRUE(rom && rom->title && *(rom->title) == ROM_TITLE);
    EXPECT_TRUE(rom && rom->year && *(rom->year) == ROM_YEAR);
    EXPECT_FALSE(rom && rom->manufacturer);
    EXPECT_TRUE(rom && rom->isBios && *(rom->isBios) == ROM_ISBIOS);

    // This rom should miss bios
    rom = romDB.find(ROM_PATH_NOBIOS);
    EXPECT_TRUE(rom && rom->title && *(rom->title) == ROM_TITLE);
    EXPECT_TRUE(rom && rom->year && *(rom->year) == ROM_YEAR);
    EXPECT_TRUE(rom && rom->manufacturer && *(rom->manufacturer) == ROM_MANUFACTURER);
    EXPECT_FALSE(rom && rom->isBios);

    // This rom should not be found as it lacks a name
    rom = romDB.find(ROM_PATH_NONAME);
    EXPECT_FALSE(rom);

    // This rom should not be found as it lacks info
    rom = romDB.find(ROM_PATH_NOINFO);
    EXPECT_FALSE(rom);
}
