#include "inputmanager.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

InputManager::InputManager()
{
    // We surely have a keyboard as a first input method
    addDevice(Input::Device{Input::Device::Type::Keyboard, 0, "Standard Keyboard"});

    // Finding connected joysticks
    for (unsigned int it = 0; it < MAX_JOYSTICK; it++)
    {
        if (sf::Joystick::isConnected(it))
        {
            auto id = sf::Joystick::getIdentification(it);
            addDevice(Input::Device{Input::Device::Type::Joystick, it, id.name});
        }
    }

    // Checking if the user wanted to close the window
    mReactions.emplace_back([](const sf::Event& event) { return event.type == sf::Event::Closed; }, &closeWindow);

    // Checking if the user pressed the EXIT button
    mReactions.emplace_back(
        [this](const sf::Event& event) {
            auto device = mAvailableInputs.begin();
            return device->checkEvent(Input::Frontend::Command::Esc, event);
        },
        &closeWindow);

    // Checking if the user wanted to go up
    mReactions.emplace_back(
        [this](const sf::Event& event) {
            auto device = mAvailableInputs.begin();
            return device->checkEvent(Input::Frontend::Command::Up, event);
        },
        &goUp);

    // Checking if the user wanted to go down
    mReactions.emplace_back(
        [this](const sf::Event& event) {
            auto device = mAvailableInputs.begin();
            return device->checkEvent(Input::Frontend::Command::Down, event);
        },
        &goDown);

    // Checking if the user wanted to select
    mReactions.emplace_back(
        [this](const sf::Event& event) {
            auto device = mAvailableInputs.begin();
            return device->checkEvent(Input::Frontend::Command::Enter, event);
        },
        &select);
}

std::list<sf::Event> InputManager::events(sf::RenderWindow& window) const
{
    std::list<sf::Event> result;
    sf::Event event{};
    while (window.pollEvent(event))
    {
        result.push_back(event);
    }

    return result;
}

void InputManager::manage(sf::RenderWindow& window)
{
    auto windowEvents = events(window);
    for (auto const& event : windowEvents)
    {
        for (auto const& reaction : mReactions)
        {
            if (reaction.t(event))
            {
                (*reaction.r)();
            }
        }

        // Checking if a new joystick has been connected
        if (event.type == sf::Event::JoystickConnected)
        {
            auto id = sf::Joystick::getIdentification(event.joystickConnect.joystickId);
            addDevice(Input::Device{Input::Device::Type::Joystick, event.joystickConnect.joystickId, id.name});
        }

        // Checking if a joystick has been disconnected
        if (event.type == sf::Event::JoystickDisconnected)
        {
            auto id = sf::Joystick::getIdentification(event.joystickConnect.joystickId);
            removeDevice(Input::Device{Input::Device::Type::Joystick, event.joystickConnect.joystickId, id.name});
        }
    }
}

std::vector<Input::Device> InputManager::getDevices() const
{
    return mAvailableInputs;
}

void InputManager::addDevice(const Input::Device& device)
{
    mAvailableInputs.push_back(device);
    std::ranges::sort(mAvailableInputs,
                      [](const Input::Device& first, const Input::Device& second) { return first < second; });

    auto active = mAvailableInputs.begin();
    spdlog::trace("New input device discovered: {} {} ({}), Active input device: {} {} ({})",
                  magic_enum::enum_name(device.getType()), device.getId(), device.getName(),
                  magic_enum::enum_name(active->getType()), active->getId(), active->getName());
}

void InputManager::removeDevice(const Input::Device& device)
{
    // We are using partitions instead of remove here so we can still log the removed devices
    auto removedDevice =
        std::ranges::partition(mAvailableInputs, [&device](const Input::Device& dev) { return device == dev; });
    auto active = removedDevice.begin();
    for (auto removed = mAvailableInputs.begin(); removed != active; ++removed)
    {

        spdlog::trace("Input device disconnected: {} {} ({}), Active input device: {} {} ({})",
                      magic_enum::enum_name(removed->getType()), removed->getId(), removed->getName(),
                      magic_enum::enum_name(active->getType()), active->getId(), active->getName());
    }

    mAvailableInputs.erase(mAvailableInputs.begin(), removedDevice.begin());
}
