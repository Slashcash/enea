#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <optional>
#include <string>
#include <unordered_map>

#include "rom.hpp"
#include "systemcommand.hpp"

class InputDevice;

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
        start1,
        p2_up,
        p2_down,
        p2_left,
        p2_right,
        p2_button1,
        p2_button2,
        p2_button3,
        p2_button4,
        p2_button5,
        p2_button6,
        coin2,
        start2,
        ui_pause,
        ui_cancel
    };

 private:
    static constexpr unsigned int MAX_PLAYERS = 2;

    [[nodiscard]] virtual SystemCommand::Result launch(const std::string& arguments) const;
    [[nodiscard]] virtual bool romExists(const Rom& rom) const;
    [[nodiscard]] virtual bool romIsReadable(const Rom& rom) const;
    [[nodiscard]] static std::string inputString(const InputDevice& device, const Input& input);
    [[nodiscard]] static unsigned int getNumberInputDevices(const std::vector<InputDevice>& availableInputs);
    [[nodiscard]] static unsigned int getNumberOfPlayers(const std::vector<InputDevice>& availableInputs);
    [[nodiscard]] static unsigned int mapInputToPlayerNumber(const Input& input);
    [[nodiscard]] static std::optional<InputDevice>
    mapDeviceToPlayerNumber(const std::vector<InputDevice>& availableInput, const unsigned int playerNum);
    [[nodiscard]] static std::string controlString(const std::vector<InputDevice>& availableInputs);

 public:
    enum class Error
    {
        ROM_FILE_NOT_FOUND,
        ROM_FILE_NOT_READABLE,
        ROM_PATH_INVALID,
        EMULATOR_ERROR,
        NO_VALID_INPUT
    };

    Emulator() = default;
    Emulator(const Emulator& emulator) = delete;
    Emulator(Emulator&& emulator) = delete;

    [[nodiscard]] std::optional<EmulatorInfo> info() const;
    [[nodiscard]] std::optional<Error> run(const Rom& rom, const std::vector<InputDevice>& availableInputs) const;

    Emulator& operator=(const Emulator& emulator) = delete;
    Emulator& operator=(Emulator&& emulator) = delete;

    virtual ~Emulator() = default;
};

#endif // EMULATOR_HPP
