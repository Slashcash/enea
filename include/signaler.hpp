#ifndef _SIGNALER_HPP_
#define _SIGNALER_HPP_

#include <vector>

#include "observer.hpp"

//inherit from this if your class needs to emit signals to observers

class Signaler {
    private:
        //members
        std::vector<Observer*> observers;

    public:
        //functions
        void emitSignal(const Signal& theSignal);
        void subscriveObserver(Observer* const theObserver);
};

#endif // _SIGNALER_HPP_
