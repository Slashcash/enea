#ifndef SYSTEMCOMMAND_HPP
#define SYSTEMCOMMAND_HPP

#include <ChefFun/Either.hh>
#include <System2.hpp>

#include <string>

class SystemCommand
{
 public:
    struct Output
    {
        int exitCode;
        std::string output;
    };

    enum class Error
    {
        SUCCESS = 0,
        LAUNCH_COMMAND,
    };

 private:
    std::string mCmd;

    // here so we can easily gmock it
    [[nodiscard]] virtual ChefFun::Either<SYSTEM2_RESULT, Output> launchCmd() const;

 public:
    SystemCommand() = delete;
    SystemCommand(const SystemCommand& cmd) = delete;
    SystemCommand(SystemCommand&& cmd) = delete;
    inline explicit SystemCommand(const std::string& cmd) : mCmd(cmd) {}

    [[nodiscard]] ChefFun::Either<Error, Output> launch() const;

    SystemCommand& operator=(const SystemCommand& cmd) = delete;
    SystemCommand& operator=(SystemCommand&& cmd) = delete;

    virtual ~SystemCommand() = default;
};

#endif // SYSTEMCOMMAND_HPP
