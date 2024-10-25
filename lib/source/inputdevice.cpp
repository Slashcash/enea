#include "inputdevice.hpp"

#include <fmt/core.h>
#include <magic_enum.hpp>

namespace {
const std::unordered_map<Emulator::Input, std::string> keyboardInput{
    {Emulator::Input::p1_up, "up"},         {Emulator::Input::p1_down, "down"},
    {Emulator::Input::p1_left, "left"},     {Emulator::Input::p1_right, "right"},
    {Emulator::Input::p1_button1, "7_pad"}, {Emulator::Input::p1_button2, "8_pad"},
    {Emulator::Input::p1_button3, "9_pad"}, {Emulator::Input::p1_button4, "4_pad"},
    {Emulator::Input::p1_button5, "5_pad"}, {Emulator::Input::p1_button6, "6_pad"},
    {Emulator::Input::coin1, "3"},          {Emulator::Input::start1, "1"},
    {Emulator::Input::p2_up, "w"},          {Emulator::Input::p2_down, "s"},
    {Emulator::Input::p2_left, "a"},        {Emulator::Input::p2_right, "d"},
    {Emulator::Input::p2_button1, "i"},     {Emulator::Input::p2_button2, "o"},
    {Emulator::Input::p2_button3, "p"},     {Emulator::Input::p2_button4, "j"},
    {Emulator::Input::p2_button5, "k"},     {Emulator::Input::p2_button6, "l"},
    {Emulator::Input::coin2, "4"},          {Emulator::Input::start2, "2"},
    {Emulator::Input::ui_pause, "9"},       {Emulator::Input::ui_cancel, "esc"}};

const std::unordered_map<Emulator::Input, std::string> joystickInput{
    {Emulator::Input::p1_up, "1,1,up"},     {Emulator::Input::p1_down, "1,1,down"},
    {Emulator::Input::p1_left, "1,0,left"}, {Emulator::Input::p1_right, "1,0,right"},
    {Emulator::Input::p1_button1, "a"},     {Emulator::Input::p1_button2, "b"},
    {Emulator::Input::p1_button3, "x"},     {Emulator::Input::p1_button4, "y"},
    {Emulator::Input::p1_button5, "tl"},    {Emulator::Input::p1_button6, "tr"},
    {Emulator::Input::coin1, "select"},     {Emulator::Input::start1, "start"},
    {Emulator::Input::p2_up, "a"},          {Emulator::Input::p2_down, "b"},
    {Emulator::Input::p2_left, "x"},        {Emulator::Input::p2_right, "y"},
    {Emulator::Input::p2_button1, "a"},     {Emulator::Input::p2_button2, "b"},
    {Emulator::Input::p2_button3, "x"},     {Emulator::Input::p2_button4, "y"},
    {Emulator::Input::p2_button5, "tl"},    {Emulator::Input::p2_button6, "tr"},
    {Emulator::Input::coin2, "select"},     {Emulator::Input::start2, "start"},
    {Emulator::Input::ui_pause, "thumbl"},  {Emulator::Input::ui_cancel, "mode"}};
} // namespace

InputDevice::InputDevice(const Type& type, const unsigned int id, const std::string& name)
    : mType(type), mId(id), mName(name)
{
}

InputDevice::Type InputDevice::getType() const
{
    return mType;
}

std::string InputDevice::getName() const
{
    return mName;
}

unsigned int InputDevice::getId() const
{
    return mId;
}

std::string InputDevice::getEmulatorInputString(const Emulator::Input& input) const
{
    const auto& inputMap = getType() == Type::Keyboard ? keyboardInput : joystickInput;

    if (auto elem = inputMap.find(input); elem != inputMap.end())
    {
        return fmt::format("{}[{},{}]", getEmulatorDeviceString(input), getId(), elem->second);
    }

    throw Excep(fmt::format("Possible input value not found for {}", magic_enum::enum_name(input)));
}

bool InputDevice::operator==(const InputDevice& other) const
{
    return this->mType == other.mType && this->mId == other.mId;
}

std::string InputDevice::getEmulatorDeviceString(const Emulator::Input& input) const
{
    if (getType() == Type::Joystick)
    {
        return isInputDirectional(input) ? "joystick_digital" : "joystick_button";
    }

    return "keyboard";
}

bool InputDevice::isInputDirectional(const Emulator::Input& input) const
{

    if (auto inputString = magic_enum::enum_name(input); inputString.ends_with("up") || inputString.ends_with("down") ||
                                                         inputString.ends_with("left") ||
                                                         inputString.ends_with("right"))
    {
        return true;
    }

    return false;
}
