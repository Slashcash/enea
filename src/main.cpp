#include "gui.hpp"
#include "wirelessconnection.hpp"

int main() {
    WirelessConnection::enableWireless();
    WirelessConnection::disconnect();
    WirelessConnection::disableWireless();

    /*Gui gui;
    gui.run();*/
}

