#ifndef _STATE_HPP_
#define _STATE_HPP_

#include <SFML/Graphics/RenderWindow.hpp>
#include <queue>

#include "result.hpp"
#include "observer.hpp"
#include "inputmanager.hpp"
#include "scene.hpp"
#include "fontmanager.hpp"
#include "texturemanager.hpp"
#include "signaler.hpp"
#include "soundmanager.hpp"

class State : public Observer, public Signaler { //why a state observs? well it observs various signals that is receiving (ie: from the inputmanager)
    private:
        //const
        static constexpr float SCENE_WIDTH = 1920;
        static constexpr float SCENE_HEIGHT = 1080;

        //members
        bool exit_from_state; //this variable (initially false) is set to true if the state wants to exit
        std::queue<State*> change_state; //setting this variable to a pointer will notify the gui that this state is requesting a state change (and the gui will accomodate by switching to the state pointed by this pointer)
        InputManager input_manager; //the input manager for this state

        //functions
        virtual void updateEffectively() = 0; //override this to implement a state (here you should specify how the frame should be updated in every frame ie: input handling)

    protected:
        //members
        Scene scene; //the scene for this state
        TextureManager texture_manager;
        FontManager font_manager;
        SoundManager sound_manager;
        sf::RenderWindow* window; //DO NOT CALL DELETE ON THIS POINTER this stores a pointer to the window we want to draw this state on (protected as derived should be granted with an easy access)

        //functions
        void requestStateChange(State* theNewState) { change_state.push(theNewState); }
        void exit() { exit_from_state = true; }

    public:
        //ctor
        State() = delete;
        State(sf::RenderWindow* theWindow); //construct a state specifying on which window you want to draw it on

        //functions
        bool askedToExit() const { return exit_from_state; }
        State* checkStateChangeRequest() { if( change_state.empty() ) return nullptr; else { State* to_return = change_state.front(); change_state.pop(); return to_return; } }

        //pure virtual functions
        virtual void draw() const final;
        virtual void update(const sf::Time& theElapsedTime) final; //override this to implement a state (here you should specify how the frame should be updated in every frame ie: input handling)

        //dtor
        virtual ~State() {}
};

#endif // _STATE_HPP_
