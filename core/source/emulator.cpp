#include "emulator.hpp"

#include <filesystem>
#include <sstream>
#include <utility>

#include "rom.hpp"
#include "systemcommand.hpp"

std::optional<std::string> Emulator::version() const
{
    std::optional<std::string> result;
    std::pair<int, std::string> launchResult;

    SystemCommand cmd("advmame --version");
    try
    {
        launchResult = cmd.launch();
    }
    catch ([[maybe_unused]] const SystemCommand::Exception& ex)
    {
        return result;
    }

    if (launchResult.first != 0)
    {
        return result;
    }

    auto newlinePos = launchResult.second.find("\n");
    if (newlinePos == std::string::npos)
    {
        return result;
    }
    else
    {
        return launchResult.second.substr(0, newlinePos);
    }
}

int Emulator::run(const Rom& rom) const
{
    std::string advMameCmd = "advmame";
    std::string advMameRom = rom.path().stem();

    std::stringstream cmdString;
    cmdString << advMameCmd << " --dir_rom " << rom.path().parent_path() << " " << advMameRom;
    SystemCommand cmd(cmdString.str());
    return cmd.launch().first;
}