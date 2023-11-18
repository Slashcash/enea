#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <optional>
#include <stdexcept>
#include <string>

class Rom;

class Emulator
{
 public:
    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    [[nodiscard]] inline static Emulator& get()
    {
        static Emulator emulator;
        return emulator;
    }

 protected:
    Emulator() = default;

    ~Emulator() = default;

 private:
    Emulator(Emulator& emulator) = delete;

    void operator=(const Emulator&) = delete;

 public:
    [[nodiscard]] std::optional<std::string> version() const;
    [[nodiscard]] int run(const Rom& rom) const;
};

#endif // CONFIGURATION_HPP