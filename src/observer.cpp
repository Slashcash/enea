#include "observer.hpp"

#include "signaler.hpp"

void Observer::subscriveToSignaler(Signaler* const theSignaler) {
    theSignaler->subscriveObserver(this);
}
