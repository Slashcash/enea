#include "romsource_mock.hpp"

#include "rom.hpp"

static const std::filesystem::path ROM_FOLDER = "/home";
static const std::filesystem::path ROM_NAME = "sf2.zip";
static const std::filesystem::path ROM_PATH = ROM_FOLDER / ROM_NAME;

TEST(romsource, path)
{
    RomSourceMock romSource(ROM_FOLDER);
    EXPECT_EQ(romSource.path(), ROM_FOLDER);
}

TEST(romsource, scan)
{
    std::list<std::filesystem::path> folderMock;
    folderMock.push_back(ROM_PATH);

    RomSourceMock romSource(ROM_FOLDER);
    EXPECT_CALL(romSource, folderExists(ROM_FOLDER)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(romSource, scanFolder(ROM_FOLDER)).WillOnce(testing::Return(folderMock));

    auto error = romSource.scan();
    ASSERT_TRUE(std::holds_alternative<std::vector<Rom>>(error));
    auto romList = std::get<std::vector<Rom>>(error);
    ASSERT_EQ(romList.size(), folderMock.size());
    EXPECT_EQ(romList.begin()->path(), ROM_PATH);
}

TEST(romsource, scan_nonExistantFolder)
{
    // Scanning a non existant folder should fail
    RomSourceMock romSource(ROM_FOLDER);
    EXPECT_CALL(romSource, folderExists(ROM_FOLDER)).WillOnce(testing::Return(std::error_code()));

    auto error = romSource.scan();
    ASSERT_TRUE(std::holds_alternative<RomSourceMock::Error>(error));
    EXPECT_EQ(std::get<RomSourceMock::Error>(error), RomSourceMock::Error::DIRECTORY_NOT_EXISTING);
}