#include "romsource_mock.hpp"

#include "configuration.hpp"
#include "rom.hpp"
#include "version.hpp"

class RomSourceFixture : public ::testing::Test
{
 protected:
    RomSourceMock romSource{ROM_FOLDER};
    std::list<std::filesystem::path> folderMock;
    nlohmann::json readJson;

    inline RomSourceFixture()
    {
        folderMock.push_back(ROM_PATH);
        readJson["path"] = ROM_FOLDER;
        readJson["version"] = projectVersion;
        readJson["lastModified"] = CACHE_MODIFIED_TIME;

        nlohmann::json roms;
        roms.emplace_back(Rom{ROM_PATH});
        readJson["roms"] = roms;
    }

    static const std::filesystem::path ROM_FOLDER;
    static const std::string ROM_NAME;
    static const std::filesystem::path ROM_PATH;
    static const std::filesystem::path CACHE_FOLDER;
    static const std::string CACHE_MODIFIED_TIME;
};

const std::filesystem::path RomSourceFixture::ROM_FOLDER = "/home";
const std::string RomSourceFixture::ROM_NAME = "sf2.zip";
const std::filesystem::path RomSourceFixture::ROM_PATH = RomSourceFixture::ROM_FOLDER / RomSourceFixture::ROM_NAME;
const std::filesystem::path RomSourceFixture::CACHE_FOLDER =
    Configuration::get().cacheDirectory() / std::to_string(std::filesystem::hash_value(ROM_FOLDER)) / "cache.json";
const std::string RomSourceFixture::CACHE_MODIFIED_TIME = "1970-01-01 00:00:00";

TEST_F(RomSourceFixture, path)
{
    EXPECT_EQ(romSource.path(), ROM_FOLDER);
}

TEST_F(RomSourceFixture, monitorWithoutCache)
{
    EXPECT_CALL(romSource, readCache(CACHE_FOLDER))
        .WillOnce(testing::Return(std::make_pair(RomSourceMock::Error::INVALID_CACHE_FILE, "")));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));
    EXPECT_CALL(romSource, writeCache(readJson, CACHE_FOLDER)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(romSource, lastCacheModification(ROM_FOLDER))
        .WillOnce(testing::Return(std::make_pair(std::nullopt, CACHE_MODIFIED_TIME)));

    auto monitorError = romSource.monitor();
    romSource.waitPendingOperations();
    EXPECT_FALSE(monitorError.has_value());
    auto romList = romSource.romList();
    ASSERT_FALSE(romList.empty());
    ASSERT_EQ(*(romList.begin()), Rom(ROM_PATH));
}

TEST_F(RomSourceFixture, monitorWithCache)
{
    EXPECT_CALL(romSource, readCache(CACHE_FOLDER)).WillOnce(testing::Return(std::make_pair(std::nullopt, readJson)));

    auto monitorError = romSource.monitor();
    EXPECT_FALSE(monitorError.has_value());
    auto romList = romSource.romList();
    ASSERT_FALSE(romList.empty());
    ASSERT_EQ(*(romList.begin()), Rom(ROM_PATH));
}

TEST_F(RomSourceFixture, monitorNonExistantFolder)
{
    EXPECT_CALL(romSource, folderExists(ROM_FOLDER)).WillOnce(testing::Return(std::error_code()));

    auto monitorError = romSource.monitor();
    ASSERT_TRUE(monitorError.has_value());
    EXPECT_EQ(monitorError.value(), RomSource::Error::INVALID_PATH);
    auto romList = romSource.romList();
    EXPECT_TRUE(romList.empty());
}

TEST_F(RomSourceFixture, alreadyMonitoring)
{
    EXPECT_CALL(romSource, readCache(CACHE_FOLDER))
        .WillOnce(testing::Return(std::make_pair(RomSourceMock::Error::INVALID_CACHE_FILE, "")));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));

    auto monitorError = romSource.monitor();
    romSource.waitPendingOperations();
    EXPECT_FALSE(monitorError.has_value());

    monitorError = romSource.monitor();
    ASSERT_TRUE(monitorError.has_value());
    EXPECT_EQ(monitorError.value(), RomSource::Error::ALREADY_MONITORED);
}