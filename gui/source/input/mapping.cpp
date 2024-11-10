#include "input/mapping.hpp"

Input::Mapping::Mapping(const std::unordered_map<Input::Emulator::Command, Input::Emulator::Button>& emulatorMap,
                        const std::unordered_map<Input::Frontend::Command, Frontend::Button>& frontendMap)
    : mEmulatorMapping(emulatorMap), mFrontendMapping(frontendMap)
{
}

std::optional<Input::Emulator::Button> Input::Mapping::getButton(Emulator::Command command) const
{
    if (auto button = mEmulatorMapping.find(command); button != mEmulatorMapping.end())
    {
        return button->second;
    }

    return std::nullopt;
}

std::optional<Input::Frontend::Button> Input::Mapping::getButton(Input::Frontend::Command command) const
{
    if (auto button = mFrontendMapping.find(command); button != mFrontendMapping.end())
    {
        return button->second;
    }

    return std::nullopt;
}
