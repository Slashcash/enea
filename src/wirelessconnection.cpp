#include "wirelessconnection.hpp"

#include <unistd.h>

#include "filesystem.hpp"

wpa_ctrl* WirelessConnection::control_interface = nullptr;
wpa_ctrl* WirelessConnection::control_interface_events = nullptr;
bool WirelessConnection::wireless_active = false;
std::atomic<bool> WirelessConnection::wireless_connected(false);
std::thread WirelessConnection::wireless_thread;
std::atomic<bool> WirelessConnection::wireless_exit(false);
std::atomic<bool> WirelessConnection::scan_requested(false);
std::atomic<bool> WirelessConnection::scan_ready(false);
std::queue<WirelessConnection::Event> WirelessConnection::event_queue;
std::mutex WirelessConnection::event_mutex;

WirelessConnection::WirelessConnection(const std::string& theSSID, const int theSignalStrength) {
    ssid = theSSID;
    signal_strength = theSignalStrength;
}

Result WirelessConnection::getInterfaceNameFromWireless(std::string& theOutput) {
    //clear the output buffer;
    theOutput.clear();

    //open proc/net/wireless in order to parse it
    std::fstream stream;
    if( !FileSystem::openStream(stream, "/proc/net/wireless", FileSystem::INPUT) ) return Result(ERR_OPEN_WIRELESS_TABLE);

    //discard two useless lines
    std::string buffer;
    std::getline(stream, buffer);
    std::getline(stream, buffer);

    //parse the correct lines
    std::size_t separator_pos;
    if( std::getline(stream, buffer) && (separator_pos = buffer.find(":")) != std::string::npos ) {
        theOutput = buffer.substr(0, separator_pos);
        stream.close();
        return Result(Result::SUCCESS);
    }

    else { stream.close(); return Result(ERR_PARSE_WIRELESS_TABLE); }
}

Result WirelessConnection::getInterfaceNameFromNet(std::string& theOutput) {
    //clear the output buffer;
    theOutput.clear();

    //open proc/net/wireless in order to parse it
    std::ifstream stream("/proc/net/dev");
    if( !stream.is_open() ) return Result(ERR_OPEN_WIRELESS_TABLE);

    //discard two useless lines
    std::string buffer;
    std::getline(stream, buffer);
    std::getline(stream, buffer);

    //parse the correct line
    bool found = false;
    std::size_t separator_pos;
    while( std::getline(stream, buffer) ) {
        if( buffer.find("w") == 0 && (separator_pos = buffer.find(":")) != std::string::npos ) {
            theOutput = buffer.substr(0, separator_pos);
            found = true;
        }
    }

    if( found ) return Result(Result::SUCCESS);
    else return Result(ERR_PARSE_WIRELESS_TABLE);
}

std::string WirelessConnection::getInterfaceName() {
    std::string buffer;
    //try from /proc/net/wireless first
    if( getInterfaceNameFromWireless(buffer) ) return buffer;
    //if it fails try from /proc/net/dev
    else if( getInterfaceNameFromNet(buffer) ) return buffer;
    //return an empty string if everything fails
    else return "";
}

Result WirelessConnection::enableWireless() {
    if( wireless_active ) {
        writeToLog("Trying to enable wireless connectivity while it is already enabled", LogWriter::Type::WARNING);
        return Result(Result::SUCCESS);
    }

    else {
        Result temp_res;

        writeToLog("Enabling wireless communication...");

        //getting the wireless interface name
        writeToLog("Getting the wireless interface name...");
        std::string if_name = getInterfaceName();
        if( if_name.empty() ) {
            temp_res.setErrorNumber(ERR_OPEN_SOCKET);
            temp_res.setDescription("Unable to get the interface name");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        //opening the communication with wpa supplicant
        writeToLog("Opening a socket to communicate with wireless "+if_name+"...");
        if( (control_interface = wpa_ctrl_open(("/var/run/wpa_supplicant/"+if_name).c_str())) == nullptr ) {
            temp_res.setErrorNumber(ERR_OPEN_SOCKET);
            temp_res.setDescription("Unable to open a socket with the wireless daemon");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        //opening another socket to comunicate in a separate thread
        writeToLog("Opening an external socket to communicate with wireless "+if_name+"...");
        if( (control_interface_events = wpa_ctrl_open(("/var/run/wpa_supplicant/"+if_name).c_str())) == nullptr ) {
            temp_res.setErrorNumber(ERR_OPEN_SOCKET);
            temp_res.setDescription("Unable to open a socket with the wireless daemon");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        //attaching the external socket to retrieve unsolicited message
        writeToLog("Attaching the external socket to retrieve unsolicited message...");
        if( wpa_ctrl_attach(control_interface_events) != 0 ) {
            temp_res.setErrorNumber(ERR_OPEN_SOCKET);
            temp_res.setDescription("Unable to attach the external socket");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        //checking if the communication is active & reliable
        if( sendRequest("PING") != "PONG" ) {
            temp_res.setErrorNumber(ERR_OPEN_SOCKET);
            temp_res.setDescription("Opened socket is unreliable");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        //checking if we are already connected to a network (A LOT OF STRING MAGIC)
        std::size_t state_pos;
        std::string to_search_for = "wpa_state=";
        std::string state;
        if( (state = sendRequest("STATUS")).empty() ) {
            temp_res.setErrorNumber(ERR_OPEN_SOCKET);
            temp_res.setDescription("Opened socket is unreliable");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        if( (state_pos = state.find(to_search_for)) == std::string::npos ) {
            temp_res.setErrorNumber(ERR_OPEN_SOCKET);
            temp_res.setDescription("Opened socket is unreliable");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        if( state.substr(state.find("=", state_pos)+1, state.find("\n", state_pos)-state_pos-to_search_for.size()) == "COMPLETED" ) wireless_connected = true;
        else wireless_connected = false;

        //starting the thread to monitor external events
        writeToLog("Starting a new thread to start monitoring external wireless events...");
        wireless_thread = std::thread(&wirelessThread);

        //Now that everything is ok we set the flag on
        wireless_active = true;
        return Result(Result::SUCCESS);
    }
}

Result WirelessConnection::disableWireless() {
    if( !wireless_active ) {
        writeToLog("Trying to disable wireless but it is already off...", LogWriter::Type::WARNING);
        return Result(Result::SUCCESS);
    }

    //setting this off will terminate the thread
    wireless_exit = true;

    //waiting for the thread to effectively terminate
    writeToLog("Waiting for the wireless thread to terminate...");
    wireless_thread.join(); //we wait for the thread to close

    //reverting back the variables
    writeToLog("Disabling wireless...");
    wpa_ctrl_close(control_interface);
    wpa_ctrl_close(control_interface_events);
    control_interface = nullptr;
    control_interface_events = nullptr;
    wireless_exit = false;
    wireless_active = false;
    wireless_connected = false;
    return Result(Result::SUCCESS);
}

std::string WirelessConnection::receiveCommand() {
    char msg_buffer[4096]; //we need a large buffer
    std::size_t response_size = sizeof(msg_buffer);
    std::string final_string;

    if( wpa_ctrl_recv(control_interface_events, msg_buffer, &response_size) != 0 ) return ""; //if we fail to receive we return an empty string
    else { final_string.append(msg_buffer, response_size-1); return final_string; } //a coherentrly built string instead
}

std::string WirelessConnection::sendRequest(const std::string& theRequest) {
    char msg_buffer[4096]; //we need a large buffer
    std::size_t response_size = sizeof(msg_buffer);
    std::string final_string;

    if( wpa_ctrl_request(control_interface, theRequest.c_str(), theRequest.size(), msg_buffer, &response_size, nullptr) != 0 )
        return "";


    else {
        final_string.append(msg_buffer, response_size-1);
        return final_string;
    }
}

std::vector<WirelessConnection> WirelessConnection::scan() {
    std::vector<WirelessConnection> to_return;

    if( !wireless_active ) {
        Result temp_res(ERR_WIRELESS_INACTIVE, "Trying to scan for wireless networks but wireless is inactive");
        writeToLog(temp_res, LogWriter::Type::ERROR);
        return to_return;
    } //if wireless is not active why even bother?

    writeToLog("Scanning for wireless networks...");

    //requesting an actual scan
    scan_requested = true;
    Result scan_result;
    if( sendRequest("SCAN") != "OK" ) {
        scan_result.setErrorNumber(ERR_SCAN);
        scan_result.setDescription("Unable to scan for wireless networks");
        writeToLog(scan_result, LogWriter::Type::ERROR);
        return to_return;
    }

    //waiting till wpa_supplicant responds to us
    while( !scan_ready ) {}
    scan_requested = false;
    scan_ready = false;

    std::string res;
    if( (res = sendRequest("SCAN_RESULTS")).empty() ) {
        scan_result.setErrorNumber(ERR_SCAN);
        scan_result.setDescription("Unable to retrieve scan results");
        writeToLog(scan_result, LogWriter::Type::ERROR);
        return to_return; //if for some reason we have no results
    }

    //and now the complicated algorithm to parse the results
    //erasing the useless first line
    res.erase(0, res.find("\n")+1);

    std::size_t newline_pos = 0;
    std::size_t start_pos = 0;
    while( newline_pos != std::string::npos ) {
        //iterating through every row
        newline_pos = res.find("\n", start_pos);
        if( newline_pos != std::string::npos ) { //if the list is ended
            std::string current_row = res.substr(start_pos, newline_pos-start_pos);

            //extracting the ssid from the row (from now on we use obnoxious string magic to parse)
            std::size_t last_tab_pos;
            std::string temp_ssid = current_row.substr(last_tab_pos = current_row.find_last_of("\t")+1, std::string::npos);

            //extracting the tags (currently not used)
            std::size_t tag_tab_pos = current_row.substr(0, last_tab_pos-1).find_last_of("\t");
            current_row.substr(tag_tab_pos+1, last_tab_pos-tag_tab_pos-2);

            //extracting the signal level
            std::size_t signal_tab_pos = current_row.substr(0, tag_tab_pos-1).find_last_of("\t");
            int temp_signal_int = std::atoi(current_row.substr(signal_tab_pos+1, tag_tab_pos-signal_tab_pos-1).c_str());

            //building the vector
            to_return.push_back(WirelessConnection(temp_ssid, temp_signal_int));

            //scrolling to the next one
            start_pos = newline_pos + 1;
        }
    }

    return to_return;
}

int WirelessConnection::getSignalStrengthAsPercentage() const {
    //the algorithm (improvable) is taken from https://stackoverflow.com/questions/15797920/how-to-convert-wifi-signal-strength-from-quality-percent-to-rssi-dbm
    if( signal_strength <= -100 ) return 0;
    else if( signal_strength >= -50 ) return 100;
    else return (signal_strength / 2) - 100;

}

Result WirelessConnection::requestDisconnection() {
    if( !wireless_active ) {
        Result temp_res(ERR_WIRELESS_INACTIVE, "Trying to disconnect to a wireless network but wireless is inactive");
        writeToLog(temp_res, LogWriter::Type::ERROR);
        return temp_res;
    }

    else if( !wireless_connected ) {
        writeToLog("Trying to disconnect to a wireless network but wireless is not connected", LogWriter::Type::WARNING);
        return Result(Result::SUCCESS);
    }

    else {
        writeToLog("Discconecting to wifi network...");

        if( sendRequest("DISCONNECT") != "OK" ) {
            Result temp_res(ERR_DISCONNECT, "Disconnect request failed");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        return Result(Result::SUCCESS);
    }
}

void WirelessConnection::wirelessThread() {
    while( !wireless_exit.load() ) {
        while( wpa_ctrl_pending(control_interface_events) ){
            std::string msg = receiveCommand();

            //react to scan results (sending a signal to the scan function that results are available)
            if( msg.find("CTRL-EVENT-SCAN-RESULTS") != std::string::npos && scan_requested.load() ) scan_ready = true; //signaling that the scan is ready

            //react to a disconnection (signaling the class that connection is off and adding the event to the queue)
            if( msg.find("CTRL-EVENT-DISCONNECTED") != std::string::npos ) {
                wireless_connected = false;
                event_mutex.lock();
                event_queue.push(DISCONNECTION);
                event_mutex.unlock();
            }

            if( msg.find("CTRL-EVENT-CONNECTED ") != std::string::npos ) {
                wireless_connected = true;
                event_mutex.lock();
                event_queue.push(CONNECTION);
                event_mutex.unlock();
            }
        }

        usleep(250*1000); //this thread will sleep since we don't want to pollute the cpu too much and this does not need to be highly responsive
    }
}

std::vector<WirelessConnection::Event> WirelessConnection::getConnectionEvents() {
    std::vector<Event> to_return;
    if( !wireless_active ) return to_return; //if it is not active why even bother?

    event_mutex.lock(); //CRITICAL
    while( !event_queue.empty() ) {
        to_return.push_back(event_queue.front());
        event_queue.pop();
    }
    event_mutex.unlock();

    return to_return;
}

Result WirelessConnection::requestConnection(const std::string& thePassword, const bool toBeSaved) {
    if( !wireless_active ) {
        Result temp_res(ERR_WIRELESS_INACTIVE, "Trying to connect to a wireless network but wireless is inactive");
        writeToLog(temp_res, LogWriter::Type::ERROR);
        return temp_res;
    }

    else {
        writeToLog("Connecting to "+ssid+"...");
        std::string connection_number = sendRequest("ADD_NETWORK"); //adding a new network (we don't consider the case in which it is already added because we're lazy)
        if( connection_number == "FAIL" ) {
            Result temp_res(ERR_CONNECT, "Unable to add the new network");
            writeToLog(temp_res, LogWriter::Type::ERROR);
            return temp_res;
        }

        sendRequest("SET_NETWORK "+connection_number+" ssid \""+ssid+"\""); //setting ssid
        sendRequest("SET_NETWORK "+connection_number+" psk \""+thePassword+"\""); //pwd
        sendRequest("ENABLE_NETWORK "+connection_number); //connecting to the new network
        if( toBeSaved ) std::cout << sendRequest("SAVE_CONFIG") << "\n"; //saving the configuration if needed
        return Result(Result::SUCCESS);
    }
}

void WirelessConnection::dhcpRequest() {
    writeToLog("Requesting an ip with dhcp...");
    system(("dhclient -v "+getInterfaceName()).c_str()); //issuing a dhcp
}

Result WirelessConnection::requestConnection() {
    sendRequest("RECONNECT");
    return Result(Result::SUCCESS);
}
