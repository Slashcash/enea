#include "romsource_mock.hpp"

#include <fmt/format.h>

#include "softwareinfo.hpp"

static const std::filesystem::path ROM_FOLDER = std::filesystem::absolute("romfolder");
static const std::filesystem::path CACHE_FOLDER = std::filesystem::absolute("cachefolder");
static const std::filesystem::path CACHE_FILE =
    CACHE_FOLDER / fmt::format("{}{}", std::to_string(std::filesystem::hash_value(ROM_FOLDER)), ".json");
static const std::filesystem::path ROM_PATH = ROM_FOLDER / "sf2.zip";
static const std::filesystem::path SCREENSHOT_PATH = ROM_FOLDER / "sf2.png";
static const std::filesystem::path USELESS_PATH = ROM_FOLDER / "sf2.mp3";
static const std::filesystem::path NOT_ROM_PATH = ROM_FOLDER / "lollete.zip";
static const std::filesystem::path BIOS_PATH = ROM_FOLDER / "neogeo.zip";
static const std::string LAST_EDIT = "1970-01-01 00:00:00";

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static const bool ROM_IS_BIOS = false;

static const Rom::Info INFO_SET_COMPLETE{
    .title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};

static const Rom::Info INFO_SET_BIOS{.title = "NeoGeo", .year = "1991", .manufacturer = "SNK", .isBios = true};

static const nlohmann::json CACHE_JSON{{RomSource::LASTMODIFIED_JSON_FIELD, LAST_EDIT},
                                       {RomSource::VERSION_JSON_FIELD, projectVersion},
                                       {
                                           RomSource::ROMS_JSON_FIELD,
                                           {Rom::Game{ROM_PATH, INFO_SET_COMPLETE, Rom::Media{SCREENSHOT_PATH}}},
                                       }};

static const nlohmann::json EMPTY_CACHE_JSON{{RomSource::LASTMODIFIED_JSON_FIELD, LAST_EDIT},
                                             {RomSource::VERSION_JSON_FIELD, projectVersion},
                                             {
                                                 RomSource::ROMS_JSON_FIELD,
                                                 {},
                                             }};

static const std::vector<std::filesystem::path> folderMock{ROM_PATH, SCREENSHOT_PATH, NOT_ROM_PATH, USELESS_PATH,
                                                           BIOS_PATH};
static const std::vector<std::filesystem::path> emptyFolderMock{};

class RomSourceFixture : public ::testing::Test
{
 protected:
    RomSourceMock romSource{ROM_FOLDER, CACHE_FOLDER};
};

TEST_F(RomSourceFixture, scanNoCache)
{
    /*
        Ask for a scan on a rom source which is not a folder.
        Expectation: returned folder is empty
    */
    EXPECT_EQ(romSource.romFolder(), ROM_FOLDER);
    EXPECT_EQ(romSource.cacheFolder(), CACHE_FOLDER);

    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(false));
    EXPECT_TRUE(romSource.scan().empty());

    /*
        Ask for a scan on a rom source with no cache available.
        Expectation: we physically scan the folder and retrieve information from it.
            - sf2.zip is an existing rom: it will be returned by the scan with all the information associated to it
            - sf2.mp3 it's a music file: it will not be returned
            - lollete.zip: looks like a rom but it's not in our database, will not be returned
            - neogeo.zip: is a bios file, will not be returned
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));

    EXPECT_CALL(romSource, romInfo(ROM_PATH)).WillOnce(testing::Return(INFO_SET_COMPLETE));
    EXPECT_CALL(romSource, romInfo(NOT_ROM_PATH)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(romSource, romInfo(BIOS_PATH)).WillOnce(testing::Return(INFO_SET_BIOS));

    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    auto roms = romSource.scan();
    ASSERT_EQ(roms.size(), 1);
    const auto& rom = roms[0];
    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.info().title == INFO_SET_COMPLETE.title);
    EXPECT_TRUE(rom.info().year && *(rom.info().year) == INFO_SET_COMPLETE.year);
    EXPECT_TRUE(rom.info().manufacturer && *(rom.info().manufacturer) == INFO_SET_COMPLETE.manufacturer);

    /*
        Ask for a scan on an empty rom source with no cache available.
        Expectation: we correctly return an empty vector.
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(emptyFolderMock));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    EXPECT_TRUE(romSource.scan().empty());

    /*
        Ask for a scan on an empty rom source with no cache available, we fail to retrieve folder modification time.
        Expectation: we throw.
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(romSource.scan(), RomSource::Exception);
}

TEST_F(RomSourceFixture, scanCache)
{
    /*
        Ask for a scan on a rom source with cache available.
        Expectation: we read the cache correctly.
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    EXPECT_CALL(romSource, readCacheFile(CACHE_FILE)).WillOnce(testing::Return(CACHE_JSON));
    EXPECT_CALL(romSource, version()).WillOnce(testing::Return(projectVersion));
    auto roms = romSource.scan();
    ASSERT_EQ(roms.size(), 1);
    const auto& rom = roms[0];
    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.info().title == INFO_SET_COMPLETE.title);
    EXPECT_TRUE(rom.info().year && *(rom.info().year) == INFO_SET_COMPLETE.year);
    EXPECT_TRUE(rom.info().manufacturer && *(rom.info().manufacturer) == INFO_SET_COMPLETE.manufacturer);

    /*
        Ask for a scan on a rom source with cache unavailable.
        Expectation: we resort to physically scanning the folder.
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    EXPECT_CALL(romSource, readCacheFile(CACHE_FILE)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(emptyFolderMock));
    EXPECT_EQ(romSource.scan().size(), 0);

    /*
        Ask for a scan on a rom source with cache available but produced with an older version.
        Expectation: we resort to physically scanning the folder.
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    EXPECT_CALL(romSource, readCacheFile(CACHE_FILE)).WillOnce(testing::Return(CACHE_JSON));
    EXPECT_CALL(romSource, version()).WillOnce(testing::Return("oldversion"));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(emptyFolderMock));
    EXPECT_EQ(romSource.scan().size(), 0);

    /*
        Ask for a scan on a rom source with cache available but modified since last check.
        Expectation: we resort to physically scanning the folder.
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return("1969-01-01 00:00:00"));
    EXPECT_CALL(romSource, readCacheFile(CACHE_FILE)).WillOnce(testing::Return(CACHE_JSON));
    EXPECT_CALL(romSource, version()).WillOnce(testing::Return(projectVersion));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(emptyFolderMock));
    EXPECT_EQ(romSource.scan().size(), 0);
}

TEST_F(RomSourceFixture, saveOnCache)
{
    /*
        Ask for a scan then for a save in cache.
        Expectation: we correctly write the cache on file
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));

    EXPECT_CALL(romSource, romInfo(ROM_PATH)).WillOnce(testing::Return(INFO_SET_COMPLETE));
    EXPECT_CALL(romSource, romInfo(NOT_ROM_PATH)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(romSource, romInfo(BIOS_PATH)).WillOnce(testing::Return(INFO_SET_BIOS));

    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    EXPECT_CALL(romSource, version()).WillOnce(testing::Return(projectVersion));
    EXPECT_CALL(romSource, writeCacheFile(CACHE_JSON, CACHE_FILE)).WillOnce(testing::Return(true));
    EXPECT_EQ(romSource.scan().size(), 1);
    EXPECT_TRUE(romSource.saveOnCache());

    /*
        Ask for a scan in an empty folder then for a save in cache.
        Expectation: we correctly write the cache on file (with no rom)
    */
    EXPECT_CALL(romSource, isFolder(ROM_FOLDER)).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(emptyFolderMock));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    EXPECT_CALL(romSource, version()).WillOnce(testing::Return(projectVersion));
    EXPECT_CALL(romSource, writeCacheFile(EMPTY_CACHE_JSON, CACHE_FILE)).WillOnce(testing::Return(true));
    EXPECT_EQ(romSource.scan().size(), 0);
    EXPECT_TRUE(romSource.saveOnCache());

    /*
        Ask for a save in cache but writing on disk fails.
        Expectation: we return an error
    */
    EXPECT_CALL(romSource, version()).WillOnce(testing::Return(projectVersion));
    EXPECT_CALL(romSource, writeCacheFile(EMPTY_CACHE_JSON, CACHE_FILE)).WillOnce(testing::Return(false));
    EXPECT_FALSE(romSource.saveOnCache());
}
