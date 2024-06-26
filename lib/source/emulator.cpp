#include "emulator.hpp"

#include <filesystem>

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>

#include "systemcommand.hpp"

const std::unordered_map<Emulator::Input, std::string> Emulator::mKeyboardInput{
    {Input::p1_up, "up"},         {Input::p1_down, "down"},     {Input::p1_left, "left"},
    {Input::p1_right, "right"},   {Input::p1_button1, "7_pad"}, {Input::p1_button2, "8_pad"},
    {Input::p1_button3, "9_pad"}, {Input::p1_button4, "4_pad"}, {Input::p1_button5, "5_pad"},
    {Input::p1_button6, "6_pad"}, {Input::coin1, "3"},          {Input::start1, "1"},
    {Input::p2_up, "w"},          {Input::p2_down, "s"},        {Input::p2_left, "a"},
    {Input::p2_right, "d"},       {Input::p2_button1, "i"},     {Input::p2_button2, "o"},
    {Input::p2_button3, "p"},     {Input::p2_button4, "j"},     {Input::p2_button5, "k"},
    {Input::p2_button6, "l"},     {Input::coin2, "4"},          {Input::start2, "2"},
    {Input::ui_pause, "9"}};

bool Emulator::romExists(const Rom& rom) const
{
    return std::filesystem::is_regular_file(rom.path());
}

bool Emulator::romIsReadable(const Rom& rom) const
{
    namespace fs = std::filesystem;

    auto perms = fs::status(rom.path()).permissions();
    return ((perms & fs::perms::owner_read) != fs::perms::none && (perms & fs::perms::group_read) != fs::perms::none &&
            (perms & fs::perms::others_read) != fs::perms::none);
}

std::string Emulator::inputString(const Input& input)
{
    if (auto elem = mKeyboardInput.find(input); elem != mKeyboardInput.end())
    {
        return fmt::format("keyboard[0,{}]", elem->second);
    }

    throw Excep(fmt::format("Possible input value not found for {}", magic_enum::enum_name(input)));
}

std::string Emulator::controlString() const
{
    std::string result;
    magic_enum::enum_for_each<Input>([this, &result](const Input val) {
        result += fmt::format("-input_map[{}] {} ", magic_enum::enum_name(val), inputString(val));
    });
    // Erasing last character as it probably contains a space anyway
    return result.substr(0, result.size() - 1);
}

std::optional<Emulator::Error> Emulator::run(const Rom& rom) const
{
    // Checking if the file exists
    if (!romExists(rom))
    {
        return Emulator::Error::ROM_FILE_NOT_FOUND;
    }

    // Checking if the file is readable
    if (!romIsReadable(rom))
    {
        return Emulator::Error::ROM_FILE_NOT_READABLE;
    }

    // Checking if the file has stem and parent path
    auto romPath = rom.path();
    if (!romPath.has_stem() || !romPath.has_parent_path())
    {
        return Emulator::Error::ROM_PATH_INVALID;
    }

    // Launching emulator
    auto cmdString = fmt::format("-misc_quiet -nomisc_safequit {} -dir_rom {} {}", controlString(),
                                 romPath.parent_path().string(), romPath.stem().string());

    if (launch(cmdString).exitcode != 0)
    {
        return Emulator::Error::EMULATOR_ERROR;
    }

    return std::nullopt;
}

SystemCommand::Result Emulator::launch(const std::string& arguments) const
{
    SystemCommand cmd(fmt::format("advmame {}", arguments));
    return cmd.launch();
}

std::optional<Emulator::EmulatorInfo> Emulator::info() const
{
    try
    {
        auto [exitValue, launchOutput] = launch("--version");

        // If launching the advmame version fails we assume emulator is not available
        if (exitValue != 0)
        {
            return std::nullopt;
        }

        // Searching for a newline in the emulator output
        // The first line of the output will likely contain name of emulator and version
        auto newlinePos = launchOutput.find_first_of('\n');
        if (newlinePos == std::string::npos)
        {
            return std::nullopt;
        }

        // Searching for a space in the first line of the emulator output
        // The first space will likely separate emulator name and version
        auto spacePos = launchOutput.substr(0, newlinePos).find_first_of(' ');
        if (spacePos == std::string::npos)
        {
            return std::nullopt;
        }

        return std::optional<EmulatorInfo>(
            {launchOutput.substr(0, spacePos), launchOutput.substr(spacePos + 1, newlinePos - spacePos - 1)});
    }

    catch ([[maybe_unused]] const SystemCommand::Excep& ex)
    {
        return std::nullopt;
    }
};
