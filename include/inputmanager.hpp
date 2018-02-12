#ifndef _INPUTMANAGER_HPP_
#define _INPUTMANAGER_HPP_

#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "signaler.hpp"

typedef std::pair<Signal, sf::Keyboard::Key> Command; //a Command is a pair of a signal and a key, the input manager will check if a KEY is pressed and will emit the corresponding signal

class InputManager : public Signaler {
    private:
        //members
        std::vector<Command> commands;

    public:
        //ctor
        InputManager() = default;
        InputManager(const std::vector<Command>& theCommands) { commands = theCommands; }

        //getters
        std::vector<Command> getCommands() const { return commands; }

        //setters
        void setCommands(const std::vector<Command>& theCommands) { commands = theCommands; }
        void addCommand(const Command& theCommand) { commands.push_back(theCommand); }

        //various
        void checkInputs(sf::RenderWindow* win); //emits the corresponding signal if one of the COMMANDS has been registered
};

#endif // _INPUTMANAGER_HPP_
