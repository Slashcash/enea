#include "gui.hpp"

#include "mainstate.hpp"

const std::string Gui::APP_TITLE = "Enea";

Gui::Gui(const unsigned int theScreenWidth, const unsigned int theScreenHeight) {
    //before initializing the gui if we are in RELEASE mode we write the log to a file
    #ifndef NDEBUG
    redirectToFile("share/log.txt");
    #endif

    writeToLog("Starting a new application window...");
    window.create(sf::VideoMode(theScreenWidth, theScreenHeight), APP_TITLE, sf::Style::Fullscreen); //creating the actual window
    window.setFramerateLimit(25); //25 frames are really more than enough (we could switch this to 20 so this won't save some precious electrical power)
    window.setMouseCursorVisible(false);

    states.push(new MainState(&window)); //pushing the main state to the top of the stack
}

void Gui::run() {
    sf::Clock frame_clock;

    while( !states.empty() ) {
        frame_clock.restart();
        State* new_state; //if the current state requests a context switch, we will store the request here

        if( (new_state = states.top()->checkStateChangeRequest()) != nullptr ) states.push(new_state);

        if( states.top()->askedToExit() ) { delete states.top(); states.pop(); } //if the state asked to leave, pop it from the stack and delete its pointer

        else {
            states.top()->draw(); //otherwise draw it
            states.top()->update(frame_clock.getElapsedTime()); //and update it
        }
    }
}
