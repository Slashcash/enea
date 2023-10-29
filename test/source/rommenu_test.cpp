#include "rommenu.hpp"
#include "romsource_mock.hpp"

class RomMenuFixture : public ::testing::Test
{
 protected:
    RomSourceMock romSource;
    RomMenu romMenu;
    std::list<std::filesystem::path> folderMock;

    inline RomMenuFixture() : romSource(ROM_FOLDER), romMenu(romSource, sf::Font())
    {
        folderMock.push_back(ROM_PATH1);
        folderMock.push_back(ROM_PATH2);
    }

    static const std::filesystem::path ROM_FOLDER;
    static const std::string ROM_NAME1;
    static const std::string ROM_NAME2;
    static const std::filesystem::path ROM_PATH1;
    static const std::filesystem::path ROM_PATH2;
};

const std::filesystem::path RomMenuFixture::ROM_FOLDER = "/home";
const std::string RomMenuFixture::ROM_NAME1 = "sf2.zip";
const std::string RomMenuFixture::ROM_NAME2 = "mslug.zip";
const std::filesystem::path RomMenuFixture::ROM_PATH1 = RomMenuFixture::ROM_FOLDER / RomMenuFixture::ROM_NAME1;
const std::filesystem::path RomMenuFixture::ROM_PATH2 = RomMenuFixture::ROM_FOLDER / RomMenuFixture::ROM_NAME2;

TEST_F(RomMenuFixture, emptyMenu)
{
    EXPECT_FALSE(romMenu.selectedRom().has_value());
}

TEST_F(RomMenuFixture, selectedRom)
{
    EXPECT_CALL(romSource, readCache(::testing::_))
        .WillOnce(testing::Return(std::make_pair(RomSourceMock::Error::INVALID_CACHE_FILE, "")));
    EXPECT_CALL(romSource, scanFolder(::testing::_)).WillOnce(testing::Return(folderMock));

    auto error = romSource.monitor();
    ASSERT_TRUE(romMenu.selectedRom().has_value());
    EXPECT_EQ(romMenu.selectedRom().value(), Rom(ROM_PATH1));
}

TEST_F(RomMenuFixture, selectedDown)
{
    EXPECT_CALL(romSource, readCache(::testing::_))
        .WillOnce(testing::Return(std::make_pair(RomSourceMock::Error::INVALID_CACHE_FILE, "")));
    EXPECT_CALL(romSource, scanFolder(::testing::_)).WillOnce(testing::Return(folderMock));

    auto error = romSource.monitor();
    romMenu.selectedDown();
    ASSERT_TRUE(romMenu.selectedRom().has_value());
    EXPECT_EQ(romMenu.selectedRom().value(), Rom(ROM_PATH2));
}

TEST_F(RomMenuFixture, selectedUp)
{
    EXPECT_CALL(romSource, readCache(::testing::_))
        .WillOnce(testing::Return(std::make_pair(RomSourceMock::Error::INVALID_CACHE_FILE, "")));
    EXPECT_CALL(romSource, scanFolder(::testing::_)).WillOnce(testing::Return(folderMock));

    auto error = romSource.monitor();
    romMenu.selectedDown();
    romMenu.selectedUp();
    ASSERT_TRUE(romMenu.selectedRom().has_value());
    EXPECT_EQ(romMenu.selectedRom().value(), Rom(ROM_PATH1));
}
