#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <optional>
#include <string>

#include "rom.hpp"

class Emulator
{
 public:
    struct EmulatorInfo
    {
        std::string name;
        std::string version;
    };

 private:
    [[nodiscard]] virtual std::pair<int, std::string> launch(const std::string& arguments) const;
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

    /**
     * @brief Get information about emulator (name and version).
     *
     * @return Emulator information.
     */
    [[nodiscard]] std::optional<EmulatorInfo> info() const;

    /**
     * @brief Launch a rom within this emulator. This will effectively call the emulator and start the game.
     *
     * @param rom The rom to launch.
     * @return The result of the launch operation.
     */
    [[nodiscard]] std::optional<Error> run(const Rom& rom) const;

    virtual ~Emulator() = default;
};

#endif // EMULATOR_HPP