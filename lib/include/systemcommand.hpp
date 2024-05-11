#ifndef SYSTEMCOMMAND_HPP
#define SYSTEMCOMMAND_HPP

#include <string>

#include "exception.hpp"

class SystemCommand
{
 public:
    struct Result
    {
        int exitcode;
        std::string output;
    };

 private:
    std::string mCmd;

    [[nodiscard]] virtual Result launchCmd() const;

 public:
    class Excep : public Exception
    {
        using Exception::Exception;
    };

    SystemCommand() = delete;
    SystemCommand(const SystemCommand& cmd) = delete;
    SystemCommand(SystemCommand&& cmd) = delete;
    explicit SystemCommand(std::string cmd);

    [[nodiscard]] Result launch() const;

    SystemCommand& operator=(const SystemCommand& cmd) = delete;
    SystemCommand& operator=(SystemCommand&& cmd) = delete;

    virtual ~SystemCommand() = default;
};

#endif // SYSTEMCOMMAND_HPP
