#include "gui.hpp"
#include "wirelessconnection.hpp"

int main() {
    //TEST PURPOSES
    WirelessConnection::enableWireless();
    WirelessConnection::scan();
    WirelessConnection::disableWireless();

    /*Gui gui;
    gui.run();*/
}

