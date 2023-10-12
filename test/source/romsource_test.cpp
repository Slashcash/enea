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

    romSource.romAdded.connect([](const Rom& rom) { EXPECT_EQ(rom.path(), ROM_PATH); });

    auto error = romSource.monitor();
    EXPECT_FALSE(error.has_value());
}

TEST(romsource, scan_nonExistantFolder)
{
    // Scanning a non existant folder should fail
    RomSourceMock romSource(ROM_FOLDER);
    EXPECT_CALL(romSource, folderExists(ROM_FOLDER)).WillOnce(testing::Return(std::error_code()));

    auto error = romSource.monitor();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error.value(), RomSourceMock::Error::DIRECTORY_NOT_EXISTING);
}