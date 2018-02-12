#ifndef _ERRORSTATE_HPP_
#define _ERRORSTATE_HPP_

#include "state.hpp"
#include "text.hpp"

/*this state is used when the mainstate initialization fails*/

class ErrorState : public State {
    private:
        //members
        Result init_error; //this variable will store the error that this state will show on screen
        Text error_text;

        //functions
        virtual void updateEffectively();
        virtual void onNotify(const Signal& theSignal);

    public:
        //ctor
        ErrorState(sf::RenderWindow* theWindow, const Result& theError);
};

#endif // _ERRORSTATE_HPP_
