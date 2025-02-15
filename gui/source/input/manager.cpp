#include "input/manager.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>
#include <spdlog/spdlog.h>

Input::Manager::Manager()
{
    // We surely have a keyboard as a first input method
    addDevice(Input::Device(Input::Identification{Input::Type::Keyboard, "Standard Keyboard", 0, 0}, 0));

    // Finding connected joysticks
    for (unsigned int it = 0; it < MAX_JOYSTICK; it++)
    {
        if (sf::Joystick::isConnected(it))
        {
            auto id = sf::Joystick::getIdentification(it);
            Input::Identification identification{
                .type{Input::Type::Joystick}, .name{id.name}, .vendorId{id.vendorId}, .productId{id.productId}};
            spdlog::info("Querying input database for: {}", identification);
            auto mapping = Input::Database::get().find(identification);
            if (mapping.isRight() && mapping.getRight().has_value())
            {
                spdlog::info("Found predetermined mapping for: {}", identification);
                addDevice(Input::Device(identification, it, mapping.getRight().value()));
            }
            else
            {
                addDevice(Input::Device(identification, it));
            }
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

std::list<sf::Event> Input::Manager::events(sf::RenderWindow& window) const
{
    std::list<sf::Event> result;
    sf::Event event{};
    while (window.pollEvent(event))
    {
        result.push_back(event);
    }

    return result;
}

void Input::Manager::manage(sf::RenderWindow& window)
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
            Input::Identification identification{
                .type{Input::Type::Joystick}, .name{id.name}, .vendorId{id.vendorId}, .productId{id.productId}};
            spdlog::info("Querying input database for: {}", identification);
            auto mapping = Input::Database::get().find(identification);
            if (mapping.isRight() && mapping.getRight().has_value())
            {
                spdlog::info("Found predetermined mapping for: {}", identification);
                addDevice(Input::Device(identification, event.joystickConnect.joystickId, mapping.getRight().value()));
            }
            else
            {
                addDevice(Input::Device(identification, event.joystickConnect.joystickId));
            }
        }

        // Checking if a joystick has been disconnected
        if (event.type == sf::Event::JoystickDisconnected)
        {
            auto id = sf::Joystick::getIdentification(event.joystickConnect.joystickId);
            removeDevice(Input::Device(Input::Identification{Input::Type::Joystick, id.name, id.vendorId, id.productId},
                                       event.joystickConnect.joystickId));
        }
    }
}

std::vector<Input::Device> Input::Manager::getDevices() const
{
    return mAvailableInputs;
}

void Input::Manager::addDevice(const Input::Device& device)
{
    mAvailableInputs.push_back(device);
    std::ranges::sort(mAvailableInputs,
                      [](const Input::Device& first, const Input::Device& second) { return first < second; });

    auto active = mAvailableInputs.begin();
    spdlog::trace("New input device discovered: {} {} ({}), Active input device: {} {} ({})",
                  magic_enum::enum_name(device.getIdentification().type), device.getId(),
                  device.getIdentification().name, magic_enum::enum_name(active->getIdentification().type),
                  active->getId(), active->getIdentification().name);
}

void Input::Manager::removeDevice(const Input::Device& device)
{
    // We are using partitions instead of remove here so we can still log the removed devices
    auto removedDevice =
        std::ranges::partition(mAvailableInputs, [&device](const Input::Device& dev) { return device == dev; });
    auto active = removedDevice.begin();
    for (auto removed = mAvailableInputs.begin(); removed != active; ++removed)
    {

        spdlog::trace("Input device disconnected: {} {} ({}), Active input device: {} {} ({})",
                      magic_enum::enum_name(removed->getIdentification().type), removed->getId(),
                      removed->getIdentification().name, magic_enum::enum_name(active->getIdentification().type),
                      active->getId(), active->getIdentification().name);
    }

    mAvailableInputs.erase(mAvailableInputs.begin(), removedDevice.begin());
}

unsigned int Input::Manager::getNumberDevices(const std::vector<Input::Device>& availableInputs)
{
    return availableInputs.size();
}

std::string Input::Manager::inputString(const Input::Device& device, const Input::Emulator::Command& command)
{
    return device.getEmulatorInputString(command);
}

unsigned int Input::Manager::getNumberOfPlayers(const std::vector<Input::Device>& availableInputs)
{
    return std::min(getNumberDevices(availableInputs), MAX_PLAYERS);
}

std::optional<Input::Device> Input::Manager::mapDeviceToPlayerNumber(const std::vector<Input::Device>& availableInput,
                                                                     const unsigned int playerNum)
{
    // We reconstruct the vector for convenience, for our emulator everytime the
    // availableInput vector contains a keyboard it can be effectively considered as two separate
    // input device (as it can accomodate two players)
    std::vector<Input::Device> buffer;

    for (const auto& device : availableInput)
    {
        buffer.push_back(device);
    }

    std::ranges::sort(buffer);

    return playerNum > 0 && playerNum <= buffer.size() ? std::optional<Input::Device>(buffer[playerNum - 1])
                                                       : std::nullopt;
}

unsigned int Input::Manager::mapInputToPlayerNumber(const Input::Emulator::Command& command)
{
    auto inputString = magic_enum::enum_name(command);
    if (inputString.starts_with("p1") || inputString.starts_with("ui") || inputString == "coin1" ||
        inputString == "start1")
    {
        return 1;
    }
    else if (inputString.starts_with("p2") || inputString == "coin2" || inputString == "start2")
    {
        return 2;
    }
    else if (inputString.starts_with("p3") || inputString == "coin3" || inputString == "start3")
    {
        return 3;
    }
    else if (inputString.starts_with("p4") || inputString == "coin4" || inputString == "start4")
    {
        return 4;
    }
    else
    {
        throw Exception("Unsupported number of players");
    }
}

std::string Input::Manager::controlString() const
{
    std::string result;
    auto playerNum = getNumberOfPlayers(mAvailableInputs);
    magic_enum::enum_for_each<Input::Emulator::Command>(
        [this, &playerNum, &result](const Input::Emulator::Command val) {
            if (auto player = mapInputToPlayerNumber(val); player <= playerNum)
            {
                auto Device = mapDeviceToPlayerNumber(mAvailableInputs, player);
                if (Device)
                {
                    result += fmt::format("-input_map[{}] {} ", magic_enum::enum_name(val), inputString(*Device, val));
                }
            }
        });
    // Erasing last character as it probably contains a space anyway
    return result.substr(0, result.size() - 1);
}
