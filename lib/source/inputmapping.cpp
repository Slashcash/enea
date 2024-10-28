#include "inputmapping.hpp"

using namespace Input;

Mapping::Mapping(const std::unordered_map<Emulator::Command, Button>& buttonMap) : mButtonMapping(buttonMap)
{
}

Mapping::Mapping(const std::unordered_map<Input::Emulator::Command, Button>& buttonMap,
                 const std::unordered_map<unsigned int, sf::Joystick::Axis>& axisMap)
    : mButtonMapping(buttonMap), mAxisMapping(axisMap)
{
}

std::optional<Button> Mapping::getButton(Emulator::Command command) const
{
    if (auto button = mButtonMapping.find(command); button != mButtonMapping.end())
    {
        return button->second;
    }

    return std::nullopt;
}
