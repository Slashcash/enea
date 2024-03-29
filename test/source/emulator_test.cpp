#include "emulator_mock.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <utility>

static const std::string EMULATOR_NAME = "AdvanceMAME";
static const std::string EMULATOR_VERSION = "3.9-77-g5f301c84";
static const std::string EMULATOR_VERSION_OUTPUT =
    fmt::format("{} {}\nCompiled Oct 10 2023 with gcc-11.4.0", EMULATOR_NAME, EMULATOR_VERSION);
static const std::string EMULATOR_VERSION_UNEXPECTED_OUTPUT = "UNEXPECTED OUTPUT";
static const std::filesystem::path ROM_PATH = std::filesystem::absolute("sf2.zip");
static const std::filesystem::path ROM_PATH_INVALID = "";
static const std::string LAUNCH_COMMAND =
    fmt::format("advmame --dir_rom {} {}", ROM_PATH.parent_path().string(), ROM_PATH.stem().string());

class EmulatorFixture : public ::testing::Test
{
 protected:
    EmulatorMock emulator;
    Rom rom{ROM_PATH};
    Rom romPathInvalid{ROM_PATH_INVALID};
};

TEST_F(EmulatorFixture, info)
{
    EXPECT_CALL(emulator, launch("--version")).WillOnce(testing::Return(std::make_pair(0, EMULATOR_VERSION_OUTPUT)));
    auto info = emulator.info();
    ASSERT_TRUE(info.has_value());
    EXPECT_EQ((*info).name, EMULATOR_NAME);
    EXPECT_EQ((*info).version, EMULATOR_VERSION);
}

TEST_F(EmulatorFixture, infoEmulatorCallFails)
{
    EXPECT_CALL(emulator, launch("--version")).WillOnce(testing::Return(std::make_pair(1, EMULATOR_VERSION_OUTPUT)));
    auto info = emulator.info();
    ASSERT_FALSE(info.has_value());
}

TEST_F(EmulatorFixture, infoEmulatorUnexpectedOutput)
{
    EXPECT_CALL(emulator, launch("--version"))
        .WillOnce(testing::Return(std::make_pair(0, EMULATOR_VERSION_UNEXPECTED_OUTPUT)));
    auto info = emulator.info();
    ASSERT_FALSE(info.has_value());
}

TEST_F(EmulatorFixture, run)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, launch(LAUNCH_COMMAND)).WillOnce(testing::Return(std::make_pair(0, "")));

    EXPECT_FALSE(emulator.run(rom).has_value());
}

TEST_F(EmulatorFixture, runRomNonExistant)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(false));

    auto runError = emulator.run(rom);
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::ROM_FILE_NOT_FOUND);
}

TEST_F(EmulatorFixture, runRomNonReadable)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(rom)).WillOnce(testing::Return(false));

    auto runError = emulator.run(rom);
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::ROM_FILE_NOT_READABLE);
}

TEST_F(EmulatorFixture, runRomPathNoStem)
{
    EXPECT_CALL(emulator, romExists(romPathInvalid)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(romPathInvalid)).WillOnce(testing::Return(true));

    auto runError = emulator.run(romPathInvalid);
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::ROM_PATH_INVALID);
}

TEST_F(EmulatorFixture, runEmulatorError)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, launch(LAUNCH_COMMAND)).WillOnce(testing::Return(std::make_pair(1, "")));

    auto runError = emulator.run(rom);
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::EMULATOR_ERROR);
}