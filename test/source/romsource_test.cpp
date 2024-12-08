#include "romsource_mock.hpp"

static const std::filesystem::path VALID_ROM_PATH = std::filesystem::absolute("sf2.zip");
static const Rom::Info VALID_ROM_INFO{.title{"Street Fighter II"}, .isBios{false}};

/*
    We start monitoring a rom source with no cache available.
    Expectation: we scan the rom source and we find all the roms.
*/
TEST(RomSource, scan)
{
    const std::filesystem::path INVALID_ROM_PATH = std::filesystem::absolute("lol.zip");

    const std::filesystem::path UNLAUNCHABLE_ROM_PATH = std::filesystem::absolute("neogeo.zip");
    const Rom::Info UNLAUNCHABLE_ROM_INFO{.title{"NeoGeo"}, .isBios{true}};

    const std::filesystem::path NOSCREENSHOT_ROM_PATH = std::filesystem::absolute("mslug.zip");
    const Rom::Info NOSCREENSHOT_ROM_INFO{.title{"Metal Slug"}, .isBios{false}};

    const std::filesystem::path UNKNOWN_FILE_PATH = std::filesystem::absolute("lol.bat");
    const std::filesystem::path SCREENSHOT_FILE_PATH = std::filesystem::absolute("sf2.png");

    std::vector<std::filesystem::path> fileList({VALID_ROM_PATH, INVALID_ROM_PATH, UNLAUNCHABLE_ROM_PATH,
                                                 NOSCREENSHOT_ROM_PATH, UNKNOWN_FILE_PATH, SCREENSHOT_FILE_PATH});

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(source, scan()).WillOnce(testing::Return(fileList));

    // zip extension, we expect the database to be queried
    EXPECT_CALL(source, romInfo(VALID_ROM_PATH)).WillOnce(testing::Return(VALID_ROM_INFO));

    // zip extension, we expect the database to be queried
    EXPECT_CALL(source, romInfo(INVALID_ROM_PATH)).WillOnce(testing::Return(std::nullopt));

    // zip extension, we expect the database to be queried
    EXPECT_CALL(source, romInfo(UNLAUNCHABLE_ROM_PATH)).WillOnce(testing::Return(UNLAUNCHABLE_ROM_INFO));

    // zip extension, we expect the database to be queried
    EXPECT_CALL(source, romInfo(NOSCREENSHOT_ROM_PATH)).WillOnce(testing::Return(NOSCREENSHOT_ROM_INFO));

    // bat extensione, we expect the database not to be queried
    EXPECT_CALL(source, romInfo(UNKNOWN_FILE_PATH)).Times(0);

    // png extensione, we expect the database not to be queried
    EXPECT_CALL(source, romInfo(SCREENSHOT_FILE_PATH)).Times(0);

    source.monitor();

    auto roms = source.elements();

    EXPECT_EQ(roms.size(), 2);

    // The valid rom should be completely constructed
    auto validRom = std::ranges::find(roms, Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO));
    ASSERT_TRUE(validRom != roms.end());
    EXPECT_EQ(validRom->path(), VALID_ROM_PATH);
    EXPECT_EQ(validRom->info(), VALID_ROM_INFO);
    EXPECT_EQ(validRom->media(), Rom::Media{.screenshot{SCREENSHOT_FILE_PATH}});

    // This is not a valid rom, shouldn't be listed in the source
    auto invalidRom = std::ranges::find(roms, Rom::Game(INVALID_ROM_PATH, Rom::Info{}));
    EXPECT_EQ(invalidRom, roms.end());

    // This is not a launchable rom, shouldn't be listed in the source
    auto unlaunchableRom = std::ranges::find(roms, Rom::Game(UNLAUNCHABLE_ROM_PATH, UNLAUNCHABLE_ROM_INFO));
    EXPECT_EQ(unlaunchableRom, roms.end());

    // This is not a valid rom, shouldn't be listed in the source
    auto unknownFile = std::ranges::find(roms, Rom::Game(UNKNOWN_FILE_PATH, Rom::Info{}));
    EXPECT_EQ(unknownFile, roms.end());

    // This is a screenshot, shouldn't be listed in the source
    auto screenshotFile = std::ranges::find(roms, Rom::Game(SCREENSHOT_FILE_PATH, Rom::Info{}));
    EXPECT_EQ(screenshotFile, roms.end());

    // This rom should just miss the screenshot
    auto missingScreenshotRom = std::ranges::find(roms, Rom::Game(NOSCREENSHOT_ROM_PATH, NOSCREENSHOT_ROM_INFO));
    ASSERT_TRUE(missingScreenshotRom != roms.end());
    EXPECT_EQ(missingScreenshotRom->path(), NOSCREENSHOT_ROM_PATH);
    EXPECT_EQ(missingScreenshotRom->info(), NOSCREENSHOT_ROM_INFO);
    EXPECT_EQ(missingScreenshotRom->media(), Rom::Media{.screenshot{std::nullopt}});
}

/*
    We start monitoring a source with cache available.
    Expectation: we have a correctly built source without scanning.
*/
TEST(RomSource, cache)
{
    const std::string VERSION = "version";
    const std::string LAST_MODIFIED = "lastModified";
    nlohmann::json romJson{{Rom::SourceMock::VERSION_JSON_FIELD, VERSION},
                           {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, LAST_MODIFIED},
                           {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).WillOnce(testing::Return(VERSION));
    EXPECT_CALL(source, lastModified()).WillOnce(testing::Return(LAST_MODIFIED));
    EXPECT_CALL(source, scan()).Times(0);

    source.monitor();

    auto roms = source.elements();
    EXPECT_EQ(roms.size(), 1);
}

/*
    We start monitoring a source with cache available but the json is empty.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheJsonEmpty)
{
    nlohmann::json romJson;

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available but it misses the cache version.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheMissingVersion)
{
    const std::string LAST_MODIFIED = "lastModified";
    nlohmann::json romJson{{Rom::SourceMock::LASTMODIFIED_JSON_FIELD, LAST_MODIFIED},
                           {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available but the version has an invalid value.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheInvalidVersion)
{
    const std::string LAST_MODIFIED = "lastModified";
    nlohmann::json romJson{{Rom::SourceMock::VERSION_JSON_FIELD, 1},
                           {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, LAST_MODIFIED},
                           {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available but the versions are mismatching.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheMismatchingVersion)
{
    const std::string LAST_MODIFIED = "lastModified";
    nlohmann::json romJson{{Rom::SourceMock::VERSION_JSON_FIELD, "0.1"},
                           {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, LAST_MODIFIED},
                           {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).WillOnce(testing::Return("0.2"));

    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available but it misses the modified time.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheMissingLastModified)
{
    const std::string VERSION = "version";
    nlohmann::json romJson{{Rom::SourceMock::VERSION_JSON_FIELD, VERSION},
                           {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).WillOnce(testing::Return(VERSION));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available but it has invalid mofied time.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheInvalidLastModified)
{
    const std::string VERSION = "version";
    nlohmann::json romJson{{Rom::SourceMock::VERSION_JSON_FIELD, VERSION},
                           {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, 1},
                           {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).WillOnce(testing::Return(VERSION));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available bit with mismatching last modified.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheMismatchingLastModified)
{
    const std::string VERSION = "version";
    nlohmann::json romJson{{Rom::SourceMock::VERSION_JSON_FIELD, VERSION},
                           {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, "2023"},
                           {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).WillOnce(testing::Return(VERSION));
    EXPECT_CALL(source, lastModified()).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return("2024"));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available but missing roms.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheMissingRoms)
{
    const std::string VERSION = "version";
    const std::string LAST_MODIFIED = "lastModified";
    nlohmann::json romJson{
        {Rom::SourceMock::VERSION_JSON_FIELD, VERSION},
        {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, LAST_MODIFIED},
    };

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).WillOnce(testing::Return(VERSION));
    EXPECT_CALL(source, lastModified()).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return("2024"));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We start monitoring a source with cache available but invalid roms.
    Expectation: cache deemed unusable, we resort to scanning.
*/
TEST(RomSource, cacheInvalidRoms)
{
    const std::string VERSION = "version";
    const std::string LAST_MODIFIED = "lastModified";
    nlohmann::json romJson{{Rom::SourceMock::VERSION_JSON_FIELD, VERSION},
                           {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, LAST_MODIFIED},
                           {Rom::SourceMock::ROMS_JSON_FIELD, 1}};

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).WillOnce(testing::Return(VERSION));
    EXPECT_CALL(source, lastModified()).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return("2024"));
    EXPECT_CALL(source, scan()).Times(1);

    source.monitor();
}

/*
    We monitor a rom and then we try to cache the result.
    Expectation: we write the cache successfully.
*/
TEST(RomSource, writeCache)
{
    const std::string VERSION = "version";
    const std::string LAST_MODIFIED = "lastModified";
    nlohmann::json romJson{
        {Rom::SourceMock::LASTMODIFIED_JSON_FIELD, LAST_MODIFIED},
        {Rom::SourceMock::ROMS_JSON_FIELD, {Rom::Game(VALID_ROM_PATH, VALID_ROM_INFO)}},
        {Rom::SourceMock::VERSION_JSON_FIELD, VERSION},
    };

    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(romJson));
    EXPECT_CALL(source, version()).Times(testing::AtLeast(2)).WillRepeatedly(testing::Return(VERSION));
    EXPECT_CALL(source, lastModified()).WillOnce(testing::Return(LAST_MODIFIED));

    source.monitor();

    EXPECT_CALL(source, writeCacheFile(romJson, testing::_)).WillOnce(testing::Return(true));
    EXPECT_TRUE(source.writeCache());
}

/*
    We try to cache an unmonitored source.
    Expectation: we return a failure as this is meaningless.
*/
TEST(RomSource, writeCacheUnmonitoredSource)
{
    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_FALSE(source.writeCache());
}

/*
    We try to cache a source without a valid last modified time.
    Expectation: we return a failure as this is meaningless.
*/
TEST(RomSource, writeCacheInvalidSource)
{
    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(source, lastModified()).WillOnce(testing::Return(std::nullopt));

    source.monitor();
    EXPECT_FALSE(source.writeCache());
}

/*
    We try to cache a source but we fail to write on disk.
    Expectation: we return a failure.
*/
TEST(RomSource, writeCacheErrorWrite)
{
    Rom::SourceMock source("test", std::filesystem::absolute("cachedir"));
    EXPECT_CALL(source, readCacheFile(testing::_)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(source, lastModified()).WillOnce(testing::Return("2024"));

    source.monitor();
    EXPECT_CALL(source, writeCacheFile(testing::_, testing::_)).WillOnce(testing::Return(false));
    EXPECT_FALSE(source.writeCache());
}
