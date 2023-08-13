#include "rom_mock.hpp"

#include <gtest/gtest.h>

static const std::filesystem::path ROM_FOLDER = "/home";
static const std::string ROM_NAME = "sf2";
static const std::string ROM_EXTENSION = ".zip";
static const std::filesystem::path ROM_PATH = ROM_FOLDER / (ROM_NAME + ROM_EXTENSION);

TEST(rom, path)
{
    RomMock rom(ROM_PATH);
    EXPECT_EQ(rom.path(), ROM_PATH);
}

TEST(rom, fileName)
{
    RomMock rom(ROM_PATH);
    EXPECT_EQ(rom.fileName(), ROM_NAME + ROM_EXTENSION);
}

TEST(rom, name)
{
    RomMock rom(ROM_PATH);
    EXPECT_EQ(rom.name(), ROM_NAME);
}

TEST(rom, launch)
{
    RomMock rom(ROM_PATH);
    EXPECT_CALL(rom, fileExists(ROM_PATH)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(rom, runEmulator(ROM_PATH)).WillOnce(testing::Return(0));

    auto error = rom.launch();
    EXPECT_FALSE(error.has_value());
}

TEST(rom, launch_relativePath)
{
    // Launching a rom constructed with a relative path should fail
    RomMock rom(ROM_NAME);

    auto error = rom.launch();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error.value(), Rom::Error::PATH_NOT_ABSOLUTE);
}

TEST(rom, launch_nonExistantFile)
{
    // Launching a rom with a non-existant file should fail
    RomMock rom(ROM_PATH);
    EXPECT_CALL(rom, fileExists(ROM_PATH)).WillOnce(testing::Return(std::error_code()));

    auto error = rom.launch();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error.value(), Rom::Error::ROM_FILE_NOT_EXISTING);
}

TEST(rom, launch_emulatorError)
{
    // launch function should return an error if the emulator fails
    RomMock rom(ROM_PATH);
    EXPECT_CALL(rom, fileExists(ROM_PATH)).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(rom, runEmulator(ROM_PATH)).WillOnce(testing::Return(1));

    auto error = rom.launch();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(error.value(), Rom::Error::EMULATOR_ERROR);
}