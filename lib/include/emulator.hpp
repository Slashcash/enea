#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <optional>
#include <string>

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

 private:
    [[nodiscard]] virtual SystemCommand::Result launch(const std::string& arguments) const;
    [[nodiscard]] virtual bool romExists(const Rom& rom) const;
    [[nodiscard]] virtual bool romIsReadable(const Rom& rom) const;

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
