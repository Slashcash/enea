#include "systemcommand.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

ChefFun::Either<SYSTEM2_RESULT, SystemCommand::Output> SystemCommand::launchCmd() const
{
    // Launching command
    System2CommandInfo commandInfo = {};
    commandInfo.RedirectOutput = true;
    if (auto result = System2CppRun(mCmd, commandInfo); result != SYSTEM2_RESULT_SUCCESS)
    {
        return ChefFun::Either<SYSTEM2_RESULT, Output>::Left(result);
    }

    // Getting command text output
    std::string output;
    if (auto result = System2CppReadFromOutput(commandInfo, output); result != SYSTEM2_RESULT_SUCCESS)
    {
        return ChefFun::Either<SYSTEM2_RESULT, Output>::Left(result);
    }

    // Getting command return code
    int returnCode;
    if (auto result = System2CppGetCommandReturnValueSync(commandInfo, returnCode); result != SYSTEM2_RESULT_SUCCESS)
    {
        return ChefFun::Either<SYSTEM2_RESULT, Output>::Left(result);
    }

    return ChefFun::Either<SYSTEM2_RESULT, Output>::Right(Output{returnCode, output});
}

ChefFun::Either<SystemCommand::Error, SystemCommand::Output> SystemCommand::launch() const
{
    const std::string launchOperationLog = fmt::format(R"(Launched command: "{}".)", mCmd);

    return launchCmd()
        .matchRight([&launchOperationLog](auto&& output) {
            spdlog::debug(R"({} Command exited with exit code: "{}", with output "{}")", launchOperationLog,
                          output.exitCode, output.output);

            return ChefFun::Either<Error, Output>::Right(output);
        })
        .matchLeft([&launchOperationLog](auto&& error) {
            spdlog::error(R"({} Operation failed, underlying subprocess library reported error: "{}")",
                          launchOperationLog, magic_enum::enum_name(error));

            return ChefFun::Either<Error, Output>::Left(Error::LAUNCH_COMMAND);
        });
}
