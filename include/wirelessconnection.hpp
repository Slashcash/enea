#ifndef _WIRELESSCONNECTION_HPP_
#define _WIRELESSCONNECTION_HPP_

#include <cstddef> //needed to define size_t in wpa_ctrl.h
#include <wpa_ctrl.h>
#include <thread>
#include <atomic>

#include "logwriter.hpp"
#include "result.hpp"

/*this class implements the wireless communication for this software, it provides some basic connect/disconnect/scan functionality
on wireless networks using wpa_supplicant
TODO: Would it be better to enable/disable radio in enable/disable functions?
*/

class WirelessConnection : public virtual LogWriter {
    private:
        //members
        std::string ssid; //the ssid for this connection

        //static members
        static std::thread wireless_thread; //the thread which will check autonomously for wireless events
        static std::atomic<bool> wireless_exit; //a variable which will be used to signal the exit to wireless_thread
        static bool wireless_active;
        static wpa_ctrl* control_interface; //the control interface for wpa_supplicant
        static wpa_ctrl* control_interface_events; //this interface is used to retrieve external event in a separate thread

        //ctor
        WirelessConnection(const std::string& theSSID); //used privately (ie: by the scan function)

        //static functions
        static Result getInterfaceNameFromWireless(std::string& theOutput); //it parses /proc/net/wireless to get the interface name
        static Result getInterfaceNameFromNet(std::string& theOutput); //it parses /proc/dev/net to get the interface name (if it fails from /wireless)
        static std::string getInterfaceName(); //call this to get the wireless interface name
        static std::string sendRequest(const std::string& theRequest); //send a command to wpa_supplicant
        static std::string receiveCommand(); //receive an unsolicited command from wpa_supplicant
        static void wirelessThread(); //this is meant to be called into a separate thread to receive events from wpa_supplicant;

    public:
        //enum
        enum Error { //a lot of things can go wrong
            ERR_OPEN_SOCKET = 1,
            ERR_OPEN_WIRELESS_TABLE,
            ERR_PARSE_WIRELESS_TABLE,
            ERR_SCAN,
        };

        //ctor
        WirelessConnection() = delete; //why someone would construct a connection, it is better to retrieve already constructed connections from scan()

        //static functions
        static Result enableWireless(); //call this before using every function of this wireless interface
        static Result disableWireless(); //call this to disable wireless operations
        static std::vector<WirelessConnection> scan();
};

#endif // _WIRELESSCONNECTION_HPP_
