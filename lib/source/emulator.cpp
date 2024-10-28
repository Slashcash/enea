#include "emulator.hpp"

#include <filesystem>

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>
#include <spdlog/spdlog.h>

#include "inputdevice.hpp"
#include "systemcommand.hpp"

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

unsigned int Emulator::getNumberDevices(const std::vector<Input::Device>& availableInputs)
{
    unsigned int result = 0;
    for (const auto& input : availableInputs)
    {
        input.getType() == Input::Device::Type::Keyboard ? result += 2 : result += 1;
    }

    return result;
}

std::optional<Input::Device> Emulator::mapDeviceToPlayerNumber(const std::vector<Input::Device>& availableInput,
                                                               const unsigned int playerNum)
{
    // We reconstruct the vector for convenience, for our emulator everytime the
    // availableInput vector contains a keyboard it can be effectively considered as two separate
    // input device (as it can accomodate two players)
    std::vector<Input::Device> buffer;

    for (const auto& device : availableInput)
    {
        buffer.push_back(device);
        if (device.getType() == Input::Device::Type::Keyboard)
        {
            buffer.push_back(device);
        }
    }

    std::ranges::sort(buffer);

    return playerNum > 0 && playerNum <= buffer.size() ? std::optional<Input::Device>(buffer[playerNum - 1])
                                                       : std::nullopt;
}

unsigned int Emulator::getNumberOfPlayers(const std::vector<Input::Device>& availableInputs)
{
    return std::min(getNumberDevices(availableInputs), MAX_PLAYERS);
}

std::string Emulator::inputString(const Input::Device& device, const Input::Emulator::Command& command)
{
    return device.getEmulatorInputString(command);
}

unsigned int Emulator::mapInputToPlayerNumber(const Input::Emulator::Command& command)
{
    auto inputString = magic_enum::enum_name(command);
    if (inputString.starts_with("p1") || inputString.starts_with("ui") || inputString == "coin1" ||
        inputString == "start1")
    {
        return 1;
    }
    else if (inputString.starts_with("p2") || inputString == "coin2" || inputString == "start2")
    {
        return 2;
    }
    else
    {
        throw Emulator::Excep("Unsupported number of players");
    }
}

std::string Emulator::controlString(const std::vector<Input::Device>& availableInputs)
{
    std::string result;
    auto playerNum = getNumberOfPlayers(availableInputs);
    magic_enum::enum_for_each<Input::Emulator::Command>(
        [&availableInputs, &playerNum, &result](const Input::Emulator::Command val) {
            if (auto player = mapInputToPlayerNumber(val); player <= playerNum)
            {
                auto Device = mapDeviceToPlayerNumber(availableInputs, player);
                if (Device)
                {
                    result += fmt::format("-input_map[{}] {} ", magic_enum::enum_name(val), inputString(*Device, val));
                }
            }
        });
    // Erasing last character as it probably contains a space anyway
    return result.substr(0, result.size() - 1);
}

std::optional<Emulator::Error> Emulator::run(const Rom& rom, const std::vector<Input::Device>& availableInputs) const
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
    if (getNumberDevices(availableInputs) == 0)
    {
        return Emulator::Error::NO_VALID_INPUT;
    }

    auto players = getNumberOfPlayers(availableInputs);
    std::string inputLog;
    for (auto it = 1; it <= players; it++)
    {
        auto device = mapDeviceToPlayerNumber(availableInputs, it);
        if (device)
        {
            inputLog += fmt::format("Player {} will use {} {} ({})", it, magic_enum::enum_name(device->getType()),
                                    device->getId(), device->getName());

            if (it < players)
            {
                inputLog += ", ";
            }
        }
    }
    spdlog::debug(inputLog);

    // Launching emulator
    auto cmdString = fmt::format("-misc_quiet -nomisc_safequit {} -dir_rom {} {}", controlString(availableInputs),
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
