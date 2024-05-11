#include "inputmanager.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

InputManager::InputManager()
{
    // Checking if the user wanted to close the window
    mReactions.emplace_back([](const sf::Event& event) { return event.type == sf::Event::Closed; }, &closeWindow);

    // Checking if the user pressed the ESC button
    mReactions.emplace_back(
        [](const sf::Event& event) {
            // NOLINTNEXTLINE
            return event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Escape;
        },
        &closeWindow);

    // Checking if the user wanted to go up
    mReactions.emplace_back(
        [](const sf::Event& event) {
            // NOLINTNEXTLINE
            return event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Up;
        },
        &goUp);

    // Checking if the user wanted to go down
    mReactions.emplace_back(
        [](const sf::Event& event) {
            // NOLINTNEXTLINE
            return event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Down;
        },
        &goDown);

    // Checking if the user wanted to select
    mReactions.emplace_back(
        [](const sf::Event& event) {
            // NOLINTNEXTLINE
            return event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Enter;
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

void InputManager::manage(sf::RenderWindow& window) const
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
    }
}
