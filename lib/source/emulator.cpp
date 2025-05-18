#include "emulator.hpp"

#include <filesystem>

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "configuration.hpp"
#include "systemcommand.hpp"

bool Emulator::romExists(const Rom::Game& rom) const
{
    return std::filesystem::is_regular_file(rom.path());
}

bool Emulator::romIsReadable(const Rom::Game& rom) const
{
    namespace fs = std::filesystem;

    auto perms = fs::status(rom.path()).permissions();
    return ((perms & fs::perms::owner_read) != fs::perms::none && (perms & fs::perms::group_read) != fs::perms::none &&
            (perms & fs::perms::others_read) != fs::perms::none);
}

std::optional<Emulator::Error> Emulator::run(const Rom::Game& rom, const std::string& inputString) const
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

    // If there is no input available we exit with an error
    if (inputString.empty())
    {
        return Emulator::Error::NO_VALID_INPUT;
    }

    // Launching emulator
    auto cmdString = fmt::format("-cfg {} -misc_quiet -nomisc_safequit --device_video sdl --device_keyboard sdl "
                                 "--device_joystick sdl {} -dir_rom {} {}",
                                 Configuration::get().advMameConfigurationFile().string(), inputString,
                                 romPath.parent_path().string(), romPath.stem().string());

    return launch(cmdString).matchRight([](auto&& output) { return std::nullopt; }).matchLeft([](auto&& error) {
        return std::optional<Emulator::Error>(Emulator::Error::EMULATOR_ERROR);
    });
}

ChefFun::Either<SystemCommand::Error, SystemCommand::Output> Emulator::launch(const std::string& arguments) const
{
    SystemCommand cmd(fmt::format("advmame {}", arguments));
    return cmd.launch();
}

std::optional<Emulator::EmulatorInfo> Emulator::info() const
{
    auto result = launch("-version");
    if (result.isLeft())
    {
        return std::nullopt;
    }

    // If launching the advmame version fails we assume emulator is not available
    if (result.getRight().exitCode != 0)
    {
        return std::nullopt;
    }

    // Searching for a newline in the emulator output
    // The first line of the output will likely contain name of emulator and version
    auto newlinePos = result.getRight().output.find_first_of('\n');
    if (newlinePos == std::string::npos)
    {
        return std::nullopt;
    }

    // Searching for a space in the first line of the emulator output
    // The first space will likely separate emulator name and version
    auto spacePos = result.getRight().output.substr(0, newlinePos).find_first_of(' ');
    if (spacePos == std::string::npos)
    {
        return std::nullopt;
    }

    return std::optional<EmulatorInfo>({result.getRight().output.substr(0, spacePos),
                                        result.getRight().output.substr(spacePos + 1, newlinePos - spacePos - 1)});
}
