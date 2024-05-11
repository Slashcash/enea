#include "inputmanager_mock.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

class InputManagerFixture : public ::testing::Test
{
 protected:
    InputManagerMock inputmanager;
    std::list<sf::Event> events;
    sf::RenderWindow window;

 public:
    inline InputManagerFixture()
    {
        sf::Event closeWindowEvent;
        closeWindowEvent.type = sf::Event::Closed;

        sf::Event goUpEvent;
        goUpEvent.type = sf::Event::KeyPressed;
        goUpEvent.key.scancode = sf::Keyboard::Scan::Up;

        sf::Event goDownEvent;
        goDownEvent.type = sf::Event::KeyPressed;
        goDownEvent.key.scancode = sf::Keyboard::Scan::Down;

        sf::Event selectEvent;
        selectEvent.type = sf::Event::KeyPressed;
        selectEvent.key.scancode = sf::Keyboard::Scan::Enter;

        sf::Event escEvent;
        escEvent.type = sf::Event::KeyPressed;
        escEvent.key.scancode = sf::Keyboard::Scan::Escape;

        events.push_back(closeWindowEvent);
        events.push_back(goUpEvent);
        events.push_back(goDownEvent);
        events.push_back(selectEvent);
        events.push_back(escEvent);
    }
};

TEST_F(InputManagerFixture, manage)
{
    EXPECT_CALL(inputmanager, events(testing::_)).WillOnce(testing::Return(events));

    unsigned int closeTester = 0;
    bool upTester = false;
    bool downTester = false;
    bool selectTester = false;

    inputmanager.closeWindow.connect([&closeTester]() { closeTester++; });
    inputmanager.goUp.connect([&upTester]() { upTester = true; });
    inputmanager.goDown.connect([&downTester]() { downTester = true; });
    inputmanager.select.connect([&selectTester]() { selectTester = true; });

    inputmanager.manage(window);
    EXPECT_EQ(closeTester, 2);
    EXPECT_TRUE(upTester);
    EXPECT_TRUE(downTester);
    EXPECT_TRUE(selectTester);
}
