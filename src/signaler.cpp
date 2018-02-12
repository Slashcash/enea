#include "signaler.hpp"

void Signaler::emitSignal(const Signal& theSignal) {
   for( auto it = observers.begin(); it < observers.end(); it++ ) (*it)->notify(theSignal);
}

void Signaler::subscriveObserver(Observer* const theObserver) {
    observers.push_back(theObserver);
}
