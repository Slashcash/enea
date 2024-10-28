#include "inputdevice.hpp"

#include <fmt/core.h>
#include <magic_enum.hpp>

using namespace Input;

const Input::Mapping Input::Device::DEFAULT_KEYBOARD_MAPPING{{
    {Emulator::Command::p1_up, Input::Button{Input::Button::Type::keyboard, "up"}},
    {Emulator::Command::p1_down, Input::Button{Input::Button::Type::keyboard, "down"}},
    {Emulator::Command::p1_left, Input::Button{Input::Button::Type::keyboard, "left"}},
    {Emulator::Command::p1_right, Input::Button{Input::Button::Type::keyboard, "right"}},
    {Emulator::Command::p1_button1, Input::Button{Input::Button::Type::keyboard, "7_pad"}},
    {Emulator::Command::p1_button2, Input::Button{Input::Button::Type::keyboard, "8_pad"}},
    {Emulator::Command::p1_button3, Input::Button{Input::Button::Type::keyboard, "9_pad"}},
    {Emulator::Command::p1_button4, Input::Button{Input::Button::Type::keyboard, "4_pad"}},
    {Emulator::Command::p1_button5, Input::Button{Input::Button::Type::keyboard, "5_pad"}},
    {Emulator::Command::p1_button6, Input::Button{Input::Button::Type::keyboard, "6_pad"}},
    {Emulator::Command::coin1, Input::Button{Input::Button::Type::keyboard, "3"}},
    {Emulator::Command::start1, Input::Button{Input::Button::Type::keyboard, "1"}},
    {Emulator::Command::p2_up, Input::Button{Input::Button::Type::keyboard, "w"}},
    {Emulator::Command::p2_down, Input::Button{Input::Button::Type::keyboard, "s"}},
    {Emulator::Command::p2_left, Input::Button{Input::Button::Type::keyboard, "a"}},
    {Emulator::Command::p2_right, Input::Button{Input::Button::Type::keyboard, "d"}},
    {Emulator::Command::p2_button1, Input::Button{Input::Button::Type::keyboard, "i"}},
    {Emulator::Command::p2_button2, Input::Button{Input::Button::Type::keyboard, "o"}},
    {Emulator::Command::p2_button3, Input::Button{Input::Button::Type::keyboard, "p"}},
    {Emulator::Command::p2_button4, Input::Button{Input::Button::Type::keyboard, "j"}},
    {Emulator::Command::p2_button5, Input::Button{Input::Button::Type::keyboard, "k"}},
    {Emulator::Command::p2_button6, Input::Button{Input::Button::Type::keyboard, "l"}},
    {Emulator::Command::coin2, Input::Button{Input::Button::Type::keyboard, "4"}},
    {Emulator::Command::start2, Input::Button{Input::Button::Type::keyboard, "2"}},
    {Emulator::Command::ui_pause, Input::Button{Input::Button::Type::keyboard, "9"}},
    {Emulator::Command::ui_cancel, Input::Button{Input::Button::Type::keyboard, "esc"}},
}};

const Input::Mapping Input::Device::DEFAULT_JOYSTICK_MAPPING{
    {
        {Emulator::Command::p1_up, Input::Button{Input::Button::Type::joystick_digital, "1,1,up"}},
        {Emulator::Command::p1_down, Input::Button{Input::Button::Type::joystick_digital, "1,1,down"}},
        {Emulator::Command::p1_left, Input::Button{Input::Button::Type::joystick_digital, "1,0,left"}},
        {Emulator::Command::p1_right, Input::Button{Input::Button::Type::joystick_digital, "1,0,right"}},
        {Emulator::Command::p1_button1, Input::Button{Input::Button::Type::joystick_button, "a"}},
        {Emulator::Command::p1_button2, Input::Button{Input::Button::Type::joystick_button, "b"}},
        {Emulator::Command::p1_button3, Input::Button{Input::Button::Type::joystick_button, "x"}},
        {Emulator::Command::p1_button4, Input::Button{Input::Button::Type::joystick_button, "y"}},
        {Emulator::Command::p1_button5, Input::Button{Input::Button::Type::joystick_button, "tl"}},
        {Emulator::Command::p1_button6, Input::Button{Input::Button::Type::joystick_button, "tr"}},
        {Emulator::Command::coin1, Input::Button{Input::Button::Type::joystick_button, "select"}},
        {Emulator::Command::start1, Input::Button{Input::Button::Type::joystick_button, "start"}},
        {Emulator::Command::p2_up, Input::Button{Input::Button::Type::joystick_digital, "1,1,up"}},
        {Emulator::Command::p2_down, Input::Button{Input::Button::Type::joystick_digital, "1,1,down"}},
        {Emulator::Command::p2_left, Input::Button{Input::Button::Type::joystick_digital, "1,0,left"}},
        {Emulator::Command::p2_right, Input::Button{Input::Button::Type::joystick_digital, "1,0,right"}},
        {Emulator::Command::p2_button1, Input::Button{Input::Button::Type::joystick_button, "a"}},
        {Emulator::Command::p2_button2, Input::Button{Input::Button::Type::joystick_button, "b"}},
        {Emulator::Command::p2_button3, Input::Button{Input::Button::Type::joystick_button, "x"}},
        {Emulator::Command::p2_button4, Input::Button{Input::Button::Type::joystick_button, "y"}},
        {Emulator::Command::p2_button5, Input::Button{Input::Button::Type::joystick_button, "tl"}},
        {Emulator::Command::p2_button6, Input::Button{Input::Button::Type::joystick_button, "tr"}},
        {Emulator::Command::coin2, Input::Button{Input::Button::Type::joystick_button, "select"}},
        {Emulator::Command::start2, Input::Button{Input::Button::Type::joystick_button, "start"}},
        {Emulator::Command::ui_pause, Input::Button{Input::Button::Type::joystick_button, "thumbl"}},
        {Emulator::Command::ui_cancel, Input::Button{Input::Button::Type::joystick_button, "mode"}},
    },
    {{1, sf::Joystick::Axis::PovY}}};

const Input::Mapping Input::Device::DEFAULT_MAPPING{DEFAULT_KEYBOARD_MAPPING};

const std::unordered_map<Input::Frontend::Command, Input::Emulator::Command>
    Input::Device::DEFAULT_FRONTEND_TO_EMULATOR_MAPPING{
        {Frontend::Command::Up, Emulator::Command::p1_up},
        {Frontend::Command::Down, Emulator::Command::p1_down},
        {Frontend::Command::Enter, Emulator::Command::start1},
        {Frontend::Command::Esc, Emulator::Command::ui_cancel},
    };

const std::unordered_map<std::string, sf::Keyboard::Key> Input::Device::DEFAULT_BUTTON_TO_FRONTEND_KEYBOARD_MAPPING{
    {"esc", sf::Keyboard::Key::Escape},
    {"down", sf::Keyboard::Key::Down},
    {"up", sf::Keyboard::Key::Up},
    {"1", sf::Keyboard::Key::Num1},
};

const std::unordered_map<std::string, unsigned int> Input::Device::DEFAULT_BUTTON_TO_FRONTEND_JOYSTICK_MAPPING{
    {"start", 9}, {"mode", 10}};

Input::Device::Device(const Type& type, const unsigned int id, const std::string& name)
    : mType(type), mId(id), mName(name)
{
    if (mType == Type::Keyboard)
    {
        mMapping = DEFAULT_KEYBOARD_MAPPING;
    }
    else if (mType == Type::Joystick)
    {
        mMapping = DEFAULT_JOYSTICK_MAPPING;
    }
}

Input::Device::Type Input::Device::getType() const
{
    return mType;
}

std::string Input::Device::getName() const
{
    return mName;
}

unsigned int Input::Device::getId() const
{
    return mId;
}

std::optional<Input::Button> Input::Device::getEmulatorButton(const Emulator::Command& command) const
{
    // At first we try to return the button in the current mapping
    if (auto mappingButton = mMapping.getButton(command); mappingButton)
    {
        return mappingButton;
    };

    // Button was not available for current mapping, try with the default associated to our current type
    Mapping typeDefaultMapping{DEFAULT_MAPPING};
    if (mType == Type::Keyboard)
    {
        typeDefaultMapping = DEFAULT_KEYBOARD_MAPPING;
    }
    else if (mType == Type::Joystick)
    {
        typeDefaultMapping = DEFAULT_JOYSTICK_MAPPING;
    }

    if (auto typeDefaultButton = typeDefaultMapping.getButton(command); typeDefaultButton)
    {
        return typeDefaultButton;
    };

    // If the type specific default didn't have it, we try a desperate and try with the keyboard default
    if (auto defaultButton = DEFAULT_MAPPING.getButton(command); defaultButton)
    {
        return defaultButton;
    };

    // We did our best to really return something :(
    return std::nullopt;
}

std::optional<Input::Button> Input::Device::getFrontendButton(const Frontend::Command& command) const
{
    auto emulatorCommand = DEFAULT_FRONTEND_TO_EMULATOR_MAPPING.find(command);

    // This shouldn't happen
    if (emulatorCommand == DEFAULT_FRONTEND_TO_EMULATOR_MAPPING.end())
    {
        return std::nullopt;
    }

    auto button = getEmulatorButton(emulatorCommand->second);

    // We tried our best to get an associated input,
    // if we failed it means that something major is going on, better to return empty
    if (!button)
    {
        return std::nullopt;
    }

    return *button;
}

std::string Input::Device::getEmulatorInputString(const Emulator::Command& command) const
{
    auto button = getEmulatorButton(command);

    // We tried our best to get an associated input,
    // if we failed it means that something major is going on, better to throw
    if (!button)
    {
        throw Input::Device::Excep(
            fmt::format("No emulator button associated to command: {}", magic_enum::enum_name(command)));
    }

    return fmt::format("{}[{},{}]", magic_enum::enum_name(button->type), getId(), button->definition);
}

bool Input::Device::operator==(const Device& other) const
{
    return this->mType == other.mType && this->mId == other.mId;
}

Input::Device::FrontendFunction Input::Device::getFrontendCheckFunction(const Frontend::Command& command,
                                                                        const sf::Event& event) const
{
    auto button = getFrontendButton(command);

    // We tried our best to get an associated input,
    // if we failed it means that something major is going on, better to throw
    if (!button)
    {
        throw Input::Device::Excep(
            fmt::format("No frontend button associated to command: {}", magic_enum::enum_name(command)));
    }

    if (mType == Type::Keyboard)
    {
        if (auto sfmlKey = DEFAULT_BUTTON_TO_FRONTEND_KEYBOARD_MAPPING.find(button->definition);
            sfmlKey != DEFAULT_BUTTON_TO_FRONTEND_KEYBOARD_MAPPING.end())
        {
            return [sfmlKey](const sf::Event& evt) {
                return evt.type == sf::Event::KeyPressed && evt.key.code == sfmlKey->second;
            };
        }

        throw Input::Device::Excep(fmt::format("No frontend sfml key associated to key: {}", button->definition));
    }

    if (mType == Type::Joystick)
    {
        if (button->type == Input::Button::Type::joystick_button)
        {
            if (auto sfmlKey = DEFAULT_BUTTON_TO_FRONTEND_JOYSTICK_MAPPING.find(button->definition);
                sfmlKey != DEFAULT_BUTTON_TO_FRONTEND_JOYSTICK_MAPPING.end())
            {
                return [this, sfmlKey](const sf::Event& evt) {
                    return evt.type == sf::Event::JoystickButtonPressed && evt.joystickButton.joystickId == getId() &&
                           evt.joystickButton.button == sfmlKey->second;
                };
            }

            throw Input::Device::Excep(fmt::format("No frontend sfml key associated to key: {}", button->definition));
        }
        if (button->type == Input::Button::Type::joystick_digital)
        {
            float range;
            command == Input::Frontend::Command::Up ? range = -100 : range = 100;
            return [range, this](const sf::Event& evt) {
                return evt.type == sf::Event::JoystickMoved && evt.joystickMove.joystickId == getId() &&
                       evt.joystickMove.axis == sf::Joystick::Axis::PovY && evt.joystickMove.position == range;
            };
        }
        else
        {
            throw Excep("Unrecognized button input type");
        }
    }

    return [](const sf::Event&) { return false; };
}

bool Input::Device::checkEvent(const Input::Frontend::Command& command, const sf::Event& event) const
{
    return getFrontendCheckFunction(command, event)(event);
}
