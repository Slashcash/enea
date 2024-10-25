#include "emulator_mock.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <utility>

#include "inputdevice.hpp"

static const std::string EMULATOR_NAME = "AdvanceMAME";
static const std::string EMULATOR_VERSION = "3.9-77-g5f301c84";
static const std::string EMULATOR_VERSION_OUTPUT =
    fmt::format("{} {}\nCompiled Oct 10 2023 with gcc-11.4.0", EMULATOR_NAME, EMULATOR_VERSION);
static const std::string EMULATOR_VERSION_UNEXPECTED_OUTPUT = "UNEXPECTED OUTPUT";
static const std::filesystem::path ROM_PATH = std::filesystem::absolute("sf2.zip");
static const std::filesystem::path ROM_PATH_INVALID = "";
static const std::string LAUNCH_COMMAND = fmt::format(
    "-misc_quiet -nomisc_safequit -input_map[p1_up] keyboard[0,up] -input_map[p1_down] keyboard[0,down] "
    "-input_map[p1_left] keyboard[0,left] -input_map[p1_right] keyboard[0,right] -input_map[p1_button1] "
    "keyboard[0,7_pad] -input_map[p1_button2] keyboard[0,8_pad] -input_map[p1_button3] keyboard[0,9_pad] "
    "-input_map[p1_button4] keyboard[0,4_pad] -input_map[p1_button5] keyboard[0,5_pad] -input_map[p1_button6] "
    "keyboard[0,6_pad] -input_map[coin1] keyboard[0,3] -input_map[start1] keyboard[0,1] -input_map[p2_up] "
    "keyboard[0,w] -input_map[p2_down] keyboard[0,s] -input_map[p2_left] keyboard[0,a] -input_map[p2_right] "
    "keyboard[0,d] -input_map[p2_button1] keyboard[0,i] -input_map[p2_button2] keyboard[0,o] -input_map[p2_button3] "
    "keyboard[0,p] -input_map[p2_button4] keyboard[0,j] -input_map[p2_button5] keyboard[0,k] -input_map[p2_button6] "
    "keyboard[0,l] -input_map[coin2] keyboard[0,4] -input_map[start2] keyboard[0,2] -input_map[ui_pause] keyboard[0,9] "
    "-input_map[ui_cancel] keyboard[0,esc]"
    "-dir_rom {} {}",
    ROM_PATH.parent_path().string(), ROM_PATH.stem().string());

class EmulatorFixture : public ::testing::Test
{
 protected:
    EmulatorMock emulator;
    Rom rom{ROM_PATH};
    Rom romPathInvalid{ROM_PATH_INVALID};
};

TEST_F(EmulatorFixture, info)
{
    EXPECT_CALL(emulator, launch("--version"))
        .WillOnce(testing::Return(SystemCommand::Result(0, EMULATOR_VERSION_OUTPUT)));
    auto info = emulator.info();
    ASSERT_TRUE(info.has_value());
    EXPECT_EQ((*info).name, EMULATOR_NAME);
    EXPECT_EQ((*info).version, EMULATOR_VERSION);
}

TEST_F(EmulatorFixture, infoEmulatorCallFails)
{
    EXPECT_CALL(emulator, launch("--version"))
        .WillOnce(testing::Return(SystemCommand::Result(1, EMULATOR_VERSION_OUTPUT)));
    auto info = emulator.info();
    ASSERT_FALSE(info.has_value());
}

TEST_F(EmulatorFixture, infoEmulatorUnexpectedOutput)
{
    EXPECT_CALL(emulator, launch("--version"))
        .WillOnce(testing::Return(SystemCommand::Result(0, EMULATOR_VERSION_UNEXPECTED_OUTPUT)));
    auto info = emulator.info();
    ASSERT_FALSE(info.has_value());
}

TEST_F(EmulatorFixture, DISABLED_run)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, launch(LAUNCH_COMMAND)).WillOnce(testing::Return(SystemCommand::Result(0, "")));

    EXPECT_FALSE(
        emulator.run(rom, std::vector<InputDevice>{InputDevice{InputDevice::Type::Keyboard, 0, "Standard Keyboard"}})
            .has_value());
}

TEST_F(EmulatorFixture, runRomNonExistant)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(false));

    auto runError =
        emulator.run(rom, std::vector<InputDevice>{InputDevice{InputDevice::Type::Keyboard, 0, "Standard Keyboard"}});
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::ROM_FILE_NOT_FOUND);
}

TEST_F(EmulatorFixture, runRomNonReadable)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(rom)).WillOnce(testing::Return(false));

    auto runError =
        emulator.run(rom, std::vector<InputDevice>{InputDevice{InputDevice::Type::Keyboard, 0, "Standard Keyboard"}});
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::ROM_FILE_NOT_READABLE);
}

TEST_F(EmulatorFixture, runRomPathNoStem)
{
    EXPECT_CALL(emulator, romExists(romPathInvalid)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(romPathInvalid)).WillOnce(testing::Return(true));

    auto runError = emulator.run(
        romPathInvalid, std::vector<InputDevice>{InputDevice{InputDevice::Type::Keyboard, 0, "Standard Keyboard"}});
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::ROM_PATH_INVALID);
}

TEST_F(EmulatorFixture, DISABLED_runEmulatorError)
{
    EXPECT_CALL(emulator, romExists(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, romIsReadable(rom)).WillOnce(testing::Return(true));
    EXPECT_CALL(emulator, launch(LAUNCH_COMMAND)).WillOnce(testing::Return(SystemCommand::Result(1, "")));

    auto runError =
        emulator.run(rom, std::vector<InputDevice>{InputDevice{InputDevice::Type::Keyboard, 0, "Standard Keyboard"}});
    ASSERT_TRUE(runError.has_value());
    EXPECT_EQ(*(runError), Emulator::Error::EMULATOR_ERROR);
}
