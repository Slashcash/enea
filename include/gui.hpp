#ifndef _GUI_HPP_
#define _GUI_HPP_

#include <SFML/Graphics/RenderWindow.hpp>
#include <stack>

#include "state.hpp"
#include "logwriter.hpp"

/*this class sets up and starts the gui*/

class Gui : public virtual LogWriter {
    private:
        //static consts
        static const unsigned int DEFAULT_WINDOW_WIDTH = 1920;
        static const unsigned int DEFAULT_WINDOW_HEIGTH = 1080;
        static const std::string APP_TITLE;

        //members
        sf::RenderWindow window; //this is our main window
        std::stack<State*> states; //this stack contains pointers to all the states (the top one is the current state we are drawing/updating

    public:
        //ctor
        Gui(const unsigned int theScreenWidth = DEFAULT_WINDOW_WIDTH, const unsigned int theScreenHeight = DEFAULT_WINDOW_HEIGTH); //this constructor sets up the screen with the specified resolution (uses default parameter if unspecified)

        //functions
        void run(); //call this to start running the gui

        //dtor
        ~Gui() { while( !states.empty() ) { delete states.top(); states.pop(); } }
};

#endif // _GUI_HPP_
