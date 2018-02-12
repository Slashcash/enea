#include "state.hpp"

State::State(sf::RenderWindow* theWindow) : scene(sf::Vector2f(SCENE_WIDTH, SCENE_HEIGHT), sf::Vector2f(SCENE_WIDTH, SCENE_HEIGHT)) {
    exit_from_state = false; //nor it exits
    window = theWindow; //setting the window that this state will perform on

    //managing the input manager (as of now these are hardcoded but nothing blocks to retrieve from this externally
    input_manager.addCommand(std::make_pair(Signal("INPUT_EXIT"), sf::Keyboard::Escape));
    input_manager.addCommand(std::make_pair(Signal("INPUT_P1DOWN"), sf::Keyboard::Down));
    input_manager.addCommand(std::make_pair(Signal("INPUT_P1UP"), sf::Keyboard::Up));
    input_manager.addCommand(std::make_pair(Signal("INPUT_P1LEFT"), sf::Keyboard::Left));
    input_manager.addCommand(std::make_pair(Signal("INPUT_P1RIGHT"), sf::Keyboard::Right));
    input_manager.addCommand(std::make_pair(Signal("INPUT_P1START"), sf::Keyboard::Num1));
    input_manager.addCommand(std::make_pair(Signal("INPUT_P1COIN"), sf::Keyboard::Num3));
    input_manager.addCommand(std::make_pair(Signal("INPUT_P1BUTTON1"), sf::Keyboard::E));

    this->subscriveToSignaler(&input_manager);

    //activating the sound manager
    sound_manager.subscriveToSignaler(this);
}

void State::update(const sf::Time& theTime) {
    updateEffectively();

    input_manager.checkInputs(window);
    scene.update(theTime);
}

void State::draw() const {
    window->clear();

    window->draw(scene);

    window->display();
}
