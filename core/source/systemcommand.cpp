#include "systemcommand.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <type_traits>

#include <array>

std::pair<int, std::string> SystemCommand::launch() const
{
    constexpr unsigned int bufferSize = 1024;

    std::array<char, bufferSize> buffer;
    std::string output;
    auto pipe = popen(mCmd.c_str(), "r");

    if (!pipe)
    {
        throw Exception("Unable to open process pipe");
    }

    while (!feof(pipe))
    {
        if (fgets(buffer.data(), bufferSize, pipe) != nullptr)
            output += buffer.data();
    }

    auto rc = pclose(pipe);
    return rc == EXIT_SUCCESS ? std::make_pair(rc, output) : std::make_pair(rc, "");
}