#include "gui.hpp"
#include "wirelessconnection.hpp"

int main() {
    WirelessConnection::enableWireless();
    WirelessConnection::scan();
    WirelessConnection::disableWireless();

    /*Gui gui;
    gui.run();*/
}

