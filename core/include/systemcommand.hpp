#ifndef SYSTEMCOMMAND_HPP
#define SYSTEMCOMMAND_HPP

#include <stdexcept>
#include <string>
#include <utility>

class SystemCommand
{
 private:
    std::string mCmd;

    std::pair<int, std::string> launchCmd() const;

 public:
    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    SystemCommand() = delete;
    inline explicit SystemCommand(const std::string& cmd) : mCmd(cmd){};

    [[nodiscard]] std::pair<int, std::string> launch() const;
};

#endif // SYSTEMCOMMAND_HPP