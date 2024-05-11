#include "romsource_mock.hpp"

static const std::filesystem::path ROM_FOLDER = std::filesystem::absolute("romfolder");
static const std::filesystem::path ROM_PATH = ROM_FOLDER / "sf2.zip";
static const std::filesystem::path SCREENSHOT_PATH = ROM_PATH / "sf2.png";
static const std::string LAST_EDIT = "1970-01-01 00:00:00";

class RomSourceFixture : public ::testing::Test
{
 protected:
    RomSourceMock romSource{ROM_FOLDER};
    std::list<std::filesystem::path> folderMock;

    inline RomSourceFixture()
    {
        folderMock.emplace_back(ROM_PATH);
        folderMock.emplace_back(SCREENSHOT_PATH);
    }
};

TEST_F(RomSourceFixture, scan)
{
    EXPECT_EQ(romSource.romFolder(), ROM_FOLDER);

    EXPECT_CALL(romSource, isRomFolderValid()).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));

    auto result = romSource.scan();
    ASSERT_EQ(result.roms.size(), 1);
    EXPECT_EQ(*(result.roms.begin()), ROM_PATH);
    ASSERT_EQ(result.media.size(), 1);
    EXPECT_EQ(*(result.media.begin()), SCREENSHOT_PATH);
    EXPECT_EQ(result.lastModified, LAST_EDIT);

    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(LAST_EDIT));
    EXPECT_EQ(romSource.lastEditTime(), LAST_EDIT);
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(romSource.lastEditTime(), RomSource::Excep);
}

TEST_F(RomSourceFixture, scanInvalidRomPath)
{
    EXPECT_CALL(romSource, isRomFolderValid()).WillOnce(testing::Return(false));
    EXPECT_THROW(romSource.scan(), RomSource::Excep);
}

TEST_F(RomSourceFixture, scanNoLastEditTime)
{
    EXPECT_CALL(romSource, isRomFolderValid()).WillOnce(testing::Return(true));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));
    EXPECT_CALL(romSource, lastFolderModification(ROM_FOLDER)).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(romSource.scan(), RomSource::Excep);
}
