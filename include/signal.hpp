#ifndef _SIGNAL_HPP_
#define _SIGNAL_HPP_

#include <string>

/*this class abstracts a signal that can be sent to objects of this engine (ie, when a button is pressed send a SIGNAL)
a SIGNAL is simply represented by its name, be sure to not have two signals with the sama name
it can only be sent by a signaler, a signal is effectively identified by its name*/

class Signal {
    private:
        //members
        std::string name;

    public:
        //ctor
        Signal(const std::string& theName) { name = theName; }

        //getters
        const std::string getName() const { return name; }
};


#endif // _SIGNAL_HPP_
