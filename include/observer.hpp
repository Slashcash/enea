#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#include "signal.hpp"

//inherit from this class if your object needs to receive signal from other objects of this engine

class Signaler;

class Observer {
    private:
        //functions
        virtual void onNotify(const Signal& theSignal) = 0; //this is the function that needs to be overridden, it should contain the reaction for every interesting signal
        virtual void notify(const Signal& theSignal) final { onNotify(theSignal); }

    public:
        //various
        virtual void subscriveToSignaler(Signaler* const theSignaler) final;

        friend class Signaler;
};



#endif // _OBSERVER_HPP_
