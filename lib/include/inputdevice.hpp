#ifndef INPUTDEVICE_HPP
#define INPUTDEVICE_HPP

#include <string>

#include "emulator.hpp"

class Emulator;

class InputDevice
{
 public:
    enum class Type
    {
        Joystick = 0,
        Keyboard = 1
    };

    class Excep : public Exception
    {
        using Exception::Exception;
    };

    InputDevice() = delete;
    explicit InputDevice(const Type& type, const unsigned int id, const std::string& name);

    [[nodiscard]] Type getType() const;
    [[nodiscard]] std::string getEmulatorInputString(const Emulator::Input& input) const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] unsigned int getId() const;

    auto operator<=>(const InputDevice& other) const
    {
        if (this->mType == other.mType)
        {
            return this->mId <=> other.mId;
        }

        return this->mType <=> other.mType;
    }

    bool operator==(const InputDevice& other) const;

 private:
    Type mType;
    unsigned int mId;
    std::string mName;

    [[nodiscard]] std::string getEmulatorDeviceString(const Emulator::Input& input) const;
    [[nodiscard]] bool isInputDirectional(const Emulator::Input& input) const;
};

#endif
