#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <optional>
#include <string>
#include <unordered_map>

#include "rom.hpp"
#include "systemcommand.hpp"

class Emulator
{
 public:
    struct EmulatorInfo
    {
        std::string name;
        std::string version;
    };

    class Excep : public Exception
    {
        using Exception::Exception;
    };

 private:
    // the values of these enums are reflecting advmame naming
    enum class Input
    {
        p1_up,
        p1_down,
        p1_left,
        p1_right,
        p1_button1,
        p1_button2,
        p1_button3,
        p1_button4,
        p1_button5,
        p1_button6,
        coin1,
        start1
    };

    static const std::unordered_map<Input, std::string> mKeyboardInput;

    [[nodiscard]] virtual SystemCommand::Result launch(const std::string& arguments) const;
    [[nodiscard]] virtual bool romExists(const Rom& rom) const;
    [[nodiscard]] virtual bool romIsReadable(const Rom& rom) const;
    [[nodiscard]] static std::string inputString(const Input& input);
    [[nodiscard]] std::string controlString() const;

 public:
    enum class Error
    {
        ROM_FILE_NOT_FOUND,
        ROM_FILE_NOT_READABLE,
        ROM_PATH_INVALID,
        EMULATOR_ERROR
    };

    Emulator() = default;
    Emulator(const Emulator& emulator) = delete;
    Emulator(Emulator&& emulator) = delete;

    [[nodiscard]] std::optional<EmulatorInfo> info() const;
    [[nodiscard]] std::optional<Error> run(const Rom& rom) const;

    Emulator& operator=(const Emulator& emulator) = delete;
    Emulator& operator=(Emulator&& emulator) = delete;

    virtual ~Emulator() = default;
};

#endif // EMULATOR_HPP
