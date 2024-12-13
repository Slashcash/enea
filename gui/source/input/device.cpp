#include "input/device.hpp"

#include <fmt/core.h>
#include <magic_enum.hpp>

using namespace Input;

const Input::Mapping Input::Device::DEFAULT_KEYBOARD_MAPPING{
    {
        {Emulator::Command::p1_up, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "w"}},
        {Emulator::Command::p1_down, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "s"}},
        {Emulator::Command::p1_left, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "a"}},
        {Emulator::Command::p1_right, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "d"}},
        {Emulator::Command::p1_button1, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "i"}},
        {Emulator::Command::p1_button2, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "o"}},
        {Emulator::Command::p1_button3, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "p"}},
        {Emulator::Command::p1_button4, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "j"}},
        {Emulator::Command::p1_button5, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "k"}},
        {Emulator::Command::p1_button6, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "l"}},
        {Emulator::Command::coin1, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "3"}},
        {Emulator::Command::start1, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "1"}},
        {Emulator::Command::p2_up, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "up"}},
        {Emulator::Command::p2_down, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "down"}},
        {Emulator::Command::p2_left, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "left"}},
        {Emulator::Command::p2_right, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "right"}},
        {Emulator::Command::p2_button1, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "7_pad"}},
        {Emulator::Command::p2_button2, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "8_pad"}},
        {Emulator::Command::p2_button3, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "9_pad"}},
        {Emulator::Command::p2_button4, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "4_pad"}},
        {Emulator::Command::p2_button5, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "5_pad"}},
        {Emulator::Command::p2_button6, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "6_pad"}},
        {Emulator::Command::coin2, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "4"}},
        {Emulator::Command::start2, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "2"}},
        {Emulator::Command::p3_up, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "w"}},
        {Emulator::Command::p3_down, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "s"}},
        {Emulator::Command::p3_left, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "a"}},
        {Emulator::Command::p3_right, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "d"}},
        {Emulator::Command::p3_button1, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "i"}},
        {Emulator::Command::p3_button2, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "o"}},
        {Emulator::Command::p3_button3, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "p"}},
        {Emulator::Command::p3_button4, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "j"}},
        {Emulator::Command::p3_button5, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "k"}},
        {Emulator::Command::p3_button6, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "l"}},
        {Emulator::Command::coin3, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "3"}},
        {Emulator::Command::start3, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "1"}},
        {Emulator::Command::p4_up, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "up"}},
        {Emulator::Command::p4_down, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "down"}},
        {Emulator::Command::p4_left, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "left"}},
        {Emulator::Command::p4_right, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "right"}},
        {Emulator::Command::p4_button1, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "7_pad"}},
        {Emulator::Command::p4_button2, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "8_pad"}},
        {Emulator::Command::p4_button3, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "9_pad"}},
        {Emulator::Command::p4_button4, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "4_pad"}},
        {Emulator::Command::p4_button5, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "5_pad"}},
        {Emulator::Command::p4_button6, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "6_pad"}},
        {Emulator::Command::coin4, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "4"}},
        {Emulator::Command::start4, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "2"}},
        {Emulator::Command::ui_pause, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "9"}},
        {Emulator::Command::ui_cancel, Input::Emulator::Button{Input::Emulator::Button::Type::keyboard, "esc"}},
    },
    {
        {Frontend::Command::Up, Input::Frontend::Button{.axis{std::nullopt},
                                                        .joystickButton{std::nullopt},
                                                        .keyboardButton{sf::Keyboard::Key::Up}}},

        {Frontend::Command::Down, Input::Frontend::Button{.axis{std::nullopt},
                                                          .joystickButton{std::nullopt},
                                                          .keyboardButton{sf::Keyboard::Key::Down}}},

        {Frontend::Command::Esc, Input::Frontend::Button{.axis{std::nullopt},
                                                         .joystickButton{std::nullopt},
                                                         .keyboardButton{sf::Keyboard::Key::Escape}}},

        {Frontend::Command::Enter, Input::Frontend::Button{.axis{std::nullopt},
                                                           .joystickButton{std::nullopt},
                                                           .keyboardButton{sf::Keyboard::Key::Num1}}},
    }};

const Input::Mapping Input::Device::DEFAULT_JOYSTICK_MAPPING{
    {
        {Emulator::Command::p1_up, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,up"}},
        {Emulator::Command::p1_down,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,down"}},
        {Emulator::Command::p1_left,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,left"}},
        {Emulator::Command::p1_right,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,right"}},
        {Emulator::Command::p1_button1, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "a"}},
        {Emulator::Command::p1_button2, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "b"}},
        {Emulator::Command::p1_button3, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "x"}},
        {Emulator::Command::p1_button4, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "y"}},
        {Emulator::Command::p1_button5, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tl"}},
        {Emulator::Command::p1_button6, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tr"}},
        {Emulator::Command::coin1, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "select"}},
        {Emulator::Command::start1, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "start"}},
        {Emulator::Command::p2_up, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,up"}},
        {Emulator::Command::p2_down,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,down"}},
        {Emulator::Command::p2_left,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,left"}},
        {Emulator::Command::p2_right,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,right"}},
        {Emulator::Command::p2_button1, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "a"}},
        {Emulator::Command::p2_button2, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "b"}},
        {Emulator::Command::p2_button3, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "x"}},
        {Emulator::Command::p2_button4, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "y"}},
        {Emulator::Command::p2_button5, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tl"}},
        {Emulator::Command::p2_button6, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tr"}},
        {Emulator::Command::coin2, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "select"}},
        {Emulator::Command::start2, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "start"}},
        {Emulator::Command::p3_up, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,up"}},
        {Emulator::Command::p3_down,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,down"}},
        {Emulator::Command::p3_left,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,left"}},
        {Emulator::Command::p3_right,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,right"}},
        {Emulator::Command::p3_button1, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "a"}},
        {Emulator::Command::p3_button2, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "b"}},
        {Emulator::Command::p3_button3, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "x"}},
        {Emulator::Command::p3_button4, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "y"}},
        {Emulator::Command::p3_button5, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tl"}},
        {Emulator::Command::p3_button6, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tr"}},
        {Emulator::Command::coin3, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "select"}},
        {Emulator::Command::start3, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "start"}},
        {Emulator::Command::p4_up, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,up"}},
        {Emulator::Command::p4_down,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,1,down"}},
        {Emulator::Command::p4_left,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,left"}},
        {Emulator::Command::p4_right,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_digital, "1,0,right"}},
        {Emulator::Command::p4_button1, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "a"}},
        {Emulator::Command::p4_button2, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "b"}},
        {Emulator::Command::p4_button3, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "x"}},
        {Emulator::Command::p4_button4, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "y"}},
        {Emulator::Command::p4_button5, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tl"}},
        {Emulator::Command::p4_button6, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "tr"}},
        {Emulator::Command::coin4, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "select"}},
        {Emulator::Command::start4, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "start"}},
        {Emulator::Command::ui_pause,
         Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "thumbl"}},
        {Emulator::Command::ui_cancel, Input::Emulator::Button{Input::Emulator::Button::Type::joystick_button, "mode"}},
    },
    {
        {Frontend::Command::Up,
         Input::Frontend::Button{.axis{Input::Frontend::Axis{.joystickAxis{sf::Joystick::Axis::PovY}, .range{-100}}},
                                 .joystickButton{std::nullopt},
                                 .keyboardButton{std::nullopt}}},

        {Frontend::Command::Down,
         Input::Frontend::Button{.axis{Input::Frontend::Axis{.joystickAxis{sf::Joystick::Axis::PovY}, .range{100}}},
                                 .joystickButton{std::nullopt},
                                 .keyboardButton{std::nullopt}}},

        {Frontend::Command::Esc,
         Input::Frontend::Button{.axis{std::nullopt}, .joystickButton{10}, .keyboardButton{std::nullopt}}},

        {Frontend::Command::Enter,
         Input::Frontend::Button{.axis{std::nullopt}, .joystickButton{9}, .keyboardButton{std::nullopt}}},
    }};

const Input::Mapping Input::Device::DEFAULT_MAPPING{DEFAULT_KEYBOARD_MAPPING};

const std::unordered_map<std::string, unsigned int> Input::Device::DEFAULT_BUTTON_TO_FRONTEND_JOYSTICK_MAPPING{
    {"start", 9}, {"mode", 10}};

Input::Device::Device(const Input::Identification& identification, const unsigned int id)
    : mId(id), mIdentification(identification)
{
    if (mIdentification.type == Type::Keyboard)
    {
        mMapping = DEFAULT_KEYBOARD_MAPPING;
    }
    else if (mIdentification.type == Type::Joystick)
    {
        mMapping = DEFAULT_JOYSTICK_MAPPING;
    }

    nlohmann::json temp({{"values", {{{"key", mIdentification}, {"info", mMapping}}}}});
}

Device::Device(const Input::Identification& identification, const unsigned int id, const Mapping& mapping)
    : mId(id), mIdentification(identification), mMapping{mapping}
{}

unsigned int Input::Device::getId() const
{
    return mId;
}

Input::Identification Input::Device::getIdentification() const
{
    return mIdentification;
}

std::optional<Input::Emulator::Button> Input::Device::getEmulatorButton(const Emulator::Command& command) const
{
    // At first we try to return the button in the current mapping
    if (auto mappingButton = mMapping.getButton(command); mappingButton)
    {
        return mappingButton;
    };

    // Button was not available for current mapping, try with the default associated to our current type
    Mapping typeDefaultMapping{DEFAULT_MAPPING};
    if (mIdentification.type == Type::Keyboard)
    {
        typeDefaultMapping = DEFAULT_KEYBOARD_MAPPING;
    }
    else if (mIdentification.type == Type::Joystick)
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

std::optional<Input::Frontend::Button> Input::Device::getFrontendButton(const Frontend::Command& command) const
{
    return mMapping.getButton(command);
}

std::string Input::Device::getEmulatorInputString(const Emulator::Command& command) const
{
    auto button = getEmulatorButton(command);

    // We tried our best to get an associated input,
    // if we failed it means that something major is going on, better to throw
    if (!button)
    {
        throw Input::Device::Exception(
            fmt::format("No emulator button associated to command: {}", magic_enum::enum_name(command)));
    }

    return fmt::format("{}[{},{}]", magic_enum::enum_name(button->type), getId(), button->definition);
}

bool Input::Device::operator==(const Device& other) const
{
    return this->mIdentification.type == other.mIdentification.type && this->mId == other.mId;
}

bool Input::Device::checkEvent(const Input::Frontend::Command& command, const sf::Event& event) const
{
    auto button = getFrontendButton(command);
    if (!button)
    {
        spdlog::warn("No button associated to frontend command");
        return false;
    }

    if (event.type == sf::Event::KeyPressed)
    {
        return button->isPressed(event);
    }

    else if ((event.type == sf::Event::JoystickButtonPressed && event.joystickButton.joystickId == mId) ||
             (event.type == sf::Event::JoystickMoved && event.joystickMove.joystickId == mId))
    {
        return button->isPressed(event);
    }

    return false;
}
