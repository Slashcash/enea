#include "inputmanager_mock.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <gtest/gtest.h>

TEST(inputmanager, closewindow)
{
    InputManagerMock inputmanager;
    sf::RenderWindow window;
    sf::Event closeWindowEvent;
    closeWindowEvent.type = sf::Event::Closed;
    std::list<sf::Event> events;
    events.push_back(closeWindowEvent);

    EXPECT_CALL(inputmanager, events(testing::_)).WillOnce(testing::Return(events));

    constexpr unsigned int tester = 0;
    unsigned int toSet = tester + 1;
    inputmanager.closeWindow.connect([&toSet]() { toSet = tester; });

    inputmanager.manage(window);

    EXPECT_EQ(toSet, tester);
}

TEST(inputmanager, up)
{
    InputManagerMock inputmanager;
    sf::RenderWindow window;
    sf::Event goUpEvent;
    goUpEvent.type = sf::Event::KeyPressed;
    goUpEvent.key.scancode = sf::Keyboard::Scan::Up;
    std::list<sf::Event> events;
    events.push_back(goUpEvent);

    EXPECT_CALL(inputmanager, events(testing::_)).WillOnce(testing::Return(events));

    constexpr unsigned int tester = 0;
    unsigned int toSet = tester + 1;
    inputmanager.goUp.connect([&toSet]() { toSet = tester; });

    inputmanager.manage(window);

    EXPECT_EQ(toSet, tester);
}

TEST(inputmanager, down)
{
    InputManagerMock inputmanager;
    sf::RenderWindow window;
    sf::Event goDownEvent;
    goDownEvent.type = sf::Event::KeyPressed;
    goDownEvent.key.scancode = sf::Keyboard::Scan::Down;
    std::list<sf::Event> events;
    events.push_back(goDownEvent);

    EXPECT_CALL(inputmanager, events(testing::_)).WillOnce(testing::Return(events));

    constexpr unsigned int tester = 0;
    unsigned int toSet = tester + 1;
    inputmanager.goDown.connect([&toSet]() { toSet = tester; });

    inputmanager.manage(window);

    EXPECT_EQ(toSet, tester);
}

TEST(inputmanager, select)
{
    InputManagerMock inputmanager;
    sf::RenderWindow window;
    sf::Event selectEvent;
    selectEvent.type = sf::Event::KeyPressed;
    selectEvent.key.scancode = sf::Keyboard::Scan::Enter;
    std::list<sf::Event> events;
    events.push_back(selectEvent);

    EXPECT_CALL(inputmanager, events(testing::_)).WillOnce(testing::Return(events));

    constexpr unsigned int tester = 0;
    unsigned int toSet = tester + 1;
    inputmanager.select.connect([&toSet]() { toSet = tester; });

    inputmanager.manage(window);

    EXPECT_EQ(toSet, tester);
}