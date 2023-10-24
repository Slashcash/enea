#include "romsource_mock.hpp"

#include "rom.hpp"

class RomSourceFixture : public ::testing::Test
{
 protected:
    RomSourceMock romSource{ROM_FOLDER};

    static const std::filesystem::path ROM_FOLDER;
    static const std::string ROM_NAME;
    static const std::filesystem::path ROM_PATH;
};

const std::filesystem::path RomSourceFixture::ROM_FOLDER = "/home";
const std::string RomSourceFixture::ROM_NAME = "sf2.zip";
const std::filesystem::path RomSourceFixture::ROM_PATH = RomSourceFixture::ROM_FOLDER / RomSourceFixture::ROM_NAME;

TEST_F(RomSourceFixture, path)
{
    EXPECT_EQ(romSource.path(), ROM_FOLDER);
}

TEST_F(RomSourceFixture, monitor)
{
    std::list<std::filesystem::path> folderMock;
    folderMock.push_back(ROM_PATH);

    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));
    EXPECT_CALL(romSource, addRom(::testing::_)).WillOnce(testing::Return());

    auto monitorError = romSource.monitor();
    EXPECT_FALSE(monitorError.has_value());
}

TEST_F(RomSourceFixture, monitorNonExistantFolder)
{
    EXPECT_CALL(romSource, folderExists(ROM_FOLDER)).WillOnce(testing::Return(std::error_code()));

    auto monitorError = romSource.monitor();
    ASSERT_TRUE(monitorError.has_value());
    EXPECT_EQ(monitorError.value(), RomSource::Error::INVALID_PATH);
}