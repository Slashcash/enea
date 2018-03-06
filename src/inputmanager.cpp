#include <SFML/Window/Event.hpp>
#include <string>

#include "inputmanager.hpp"

void InputManager::checkInputs(sf::RenderWindow* win) {
    sf::Event event;
    while (win->pollEvent(event)) {
        if(event.type == sf::Event::KeyPressed) {
            for(auto it = commands.begin(); it < commands.end(); it++)
                if( it->second == event.key.code )
                    emitSignal(it->first);
        }
    }
}

void InputManager::discardAllInputs(sf::RenderWindow* win) {
    sf::Event event;
    while (win->pollEvent(event)) {}
}
