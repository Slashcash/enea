#include "systemcommand.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>

SystemCommand::SystemCommand(std::string cmd) : mCmd(std::move(cmd))
{
}

SystemCommand::Result SystemCommand::launchCmd() const
{
    constexpr unsigned int bufferSize = 1024;

    std::array<char, bufferSize> buffer{};
    std::string output;
    auto* pipe = popen(mCmd.c_str(), "r");

    if (pipe == nullptr)
    {
        throw Exception("Unable to open process pipe");
    }

    while (feof(pipe) == 0)
    {
        if (fgets(buffer.data(), bufferSize, pipe) != nullptr)
        {
            output += buffer.data();
        }
    }

    auto returnCode = pclose(pipe);
    return Result{returnCode, output};
}

SystemCommand::Result SystemCommand::launch() const
{
    auto result = launchCmd();
    return result.exitcode == EXIT_SUCCESS ? result : Result{result.exitcode, ""};
}
