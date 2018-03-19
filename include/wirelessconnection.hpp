#ifndef _WIRELESSCONNECTION_HPP_
#define _WIRELESSCONNECTION_HPP_

#include <cstddef> //needed to define size_t in wpa_ctrl.h
#include <wpa_ctrl.h>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

#include "logwriter.hpp"
#include "result.hpp"

/*this class implements the wireless communication for this software, it provides some basic connect/disconnect/scan functionality
on wireless networks using wpa_supplicant
TODO: Would it be better to enable/disable radio in enable/disable functions?
*/

class WirelessConnection : public virtual LogWriter {
    public:
        enum Event { //these are the events that can occur to the wireless connection externally to this software
            CONNECTION,
            DISCONNECTION
        };

    private:
        //members
        std::string ssid; //the ssid for this connection
        int signal_strength; //signal strength in dbm

        //static members
        static std::queue<Event> event_queue; //here we will stores all the connection events occured
        static std::mutex event_mutex; //this protects the event_queue
        static std::thread wireless_thread; //the thread which will check autonomously for wireless events
        static std::atomic<bool> wireless_exit; //a variable which will be used to signal the exit to wireless_thread
        static bool wireless_active; //true if wireless is active
        static std::atomic<bool> wireless_connected; //true if wireless is connected to a network
        static std::atomic<bool> scan_requested;
        static std::atomic<bool> scan_ready; //two control variables that regulates the scanning function
        static wpa_ctrl* control_interface; //the control interface for wpa_supplicant
        static wpa_ctrl* control_interface_events; //this interface is used to retrieve external event in a separate thread

        //ctor
        WirelessConnection(const std::string& theSSID, const int theSignalStrength); //used privately (ie: by the scan function)

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
            ERR_DISCONNECT,
        };

        //ctor
        WirelessConnection() = delete; //why someone would construct a connection, it is better to retrieve already constructed connections from scan()

        //getters
        std::string getSSID() const { return ssid; }
        int getSignalStrength() const { return signal_strength; } //returns signal strength in dbm
        int getSignalStrengthAsPercentage() const;

        //static functions
        static Result enableWireless(); //call this before using every function of this wireless interface
        static Result disableWireless(); //call this to disable wireless operations
        static std::vector<WirelessConnection> scan();
        static std::vector<Event> getConnectionEvents(); //call this to have a vector of connection events occured since the last call of this function
        static void disconnect(); //this returns void because calling this doesn't effectively disconnects, it just asks for a disconnection that will be handled by wpa_supplicant (if something fails it writes to log)
};

#endif // _WIRELESSCONNECTION_HPP_
