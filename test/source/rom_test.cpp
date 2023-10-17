#include "rom_mock.hpp"

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

class RomFixture : public ::testing::Test
{
 protected:
    RomMock rom{ROM_PATH};
    RomMock romCompleteJson{COMPLETE_JSON};
    RomMock romIncompleteJson{INCOMPLETE_JSON};

    static const std::filesystem::path ROM_FOLDER;
    static const std::string ROM_NAME;
    static const std::string ROM_EXTENSION;
    static const std::filesystem::path ROM_PATH;
    static const nlohmann::json COMPLETE_JSON;
    static const nlohmann::json INCOMPLETE_JSON;
};

const std::filesystem::path RomFixture::ROM_FOLDER = "/home";
const std::string RomFixture::ROM_NAME = "sf2";
const std::string RomFixture::ROM_EXTENSION = ".zip";
const std::filesystem::path RomFixture::ROM_PATH =
    RomFixture::ROM_FOLDER / (RomFixture::ROM_NAME + RomFixture::ROM_EXTENSION);

using namespace nlohmann::literals;
const nlohmann::json RomFixture::COMPLETE_JSON = R"(
    {
        "path": "/sf2.zip",
        "name": "Street Fighter II"
    }
    )"_json;

const nlohmann::json RomFixture::INCOMPLETE_JSON = R"(
    {
        "path": "/sf2.zip"
    }
    )"_json;

TEST_F(RomFixture, path)
{
    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_EQ(romCompleteJson.path(), "/sf2.zip");
    EXPECT_EQ(romIncompleteJson.path(), "/sf2.zip");
}

TEST_F(RomFixture, fileName)
{
    EXPECT_EQ(rom.fileName(), ROM_NAME + ROM_EXTENSION);
    EXPECT_EQ(romCompleteJson.fileName(), "sf2.zip");
    EXPECT_EQ(romIncompleteJson.fileName(), "sf2.zip");
}

TEST_F(RomFixture, name)
{
    EXPECT_EQ(rom.name(), ROM_NAME);
    EXPECT_EQ(romCompleteJson.name(), "Street Fighter II");
    EXPECT_EQ(romIncompleteJson.name(), "sf2");
}

TEST_F(RomFixture, launch)
{
    EXPECT_CALL(rom, runEmulator(ROM_PATH)).WillOnce(testing::Return(0));

    auto error = rom.launch();
    EXPECT_FALSE(error.has_value());
}

TEST_F(RomFixture, launchEmulatorError)
{
    EXPECT_CALL(rom, runEmulator(ROM_PATH)).WillOnce(testing::Return(1));

    auto error = rom.launch();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error.value(), RomMock::Error::EMULATOR_ERROR);
}

TEST_F(RomFixture, launchNonExistantFile)
{
    EXPECT_CALL(rom, fileExists(ROM_PATH)).WillOnce(testing::Return(std::error_code()));

    auto error = rom.launch();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error.value(), RomMock::Error::INVALID_ROM_FILE);
}