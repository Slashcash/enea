#ifndef INPUTMANAGERMOCK_HPP
#define INPUTMANAGERMOCK_HPP

#include "inputmanager.hpp"

#include <gmock/gmock.h>

#include <SFML/Window/Event.hpp>

class InputManagerMock : public InputManager
{
 public:
    using InputManager::InputManager;

    MOCK_METHOD(std::list<sf::Event>, events, (sf::RenderWindow & window), (const override));
};

#endif // INPUTMANAGERMOCK_HPP
