#include "removabledisk.hpp"

#include <cstring>

#include "filesystem.hpp"

std::thread RemovableDisk::hotplug_thread;
udev* RemovableDisk::monitor_session = nullptr;
udev_monitor* RemovableDisk::monitor = nullptr;
int RemovableDisk::fd = 0;
bool RemovableDisk::hotplug_enabled = false;
std::atomic<bool> RemovableDisk::hotplug_exit(false);
std::queue<RemovableDisk::Action> RemovableDisk::hotplug_actions;
std::mutex RemovableDisk::hotplug_mutex;

RemovableDisk::RemovableDisk(const fs::path& theDevicePath, const fs::path& theSysPath) {
    device_path = theDevicePath;
    sys_path = theSysPath;
}

std::vector<udev_device*> RemovableDisk::getDeviceChildren(udev* theSession, udev_device* theParent, std::string theSubsystem) {
    std::vector<udev_device*> buffer; //the buffer which we will return
    udev_enumerate* enumerate = udev_enumerate_new(theSession);

    //scanning for the specified subsystem
    udev_enumerate_add_match_parent(enumerate, theParent);
    udev_enumerate_add_match_subsystem(enumerate, theSubsystem.c_str());
    udev_enumerate_scan_devices(enumerate);

    udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry* entry;

    udev_list_entry_foreach(entry, devices) {
        const char* path = udev_list_entry_get_name(entry);
        udev_device* child = udev_device_new_from_syspath(theSession, path);
        buffer.push_back(child);
    }

    udev_enumerate_unref(enumerate);
    return buffer;
}

bool RemovableDisk::isRemovableDisk(udev* theSession, udev_device* theDevice) {
    bool return_value;
    std::vector<udev_device*> blocks = getDeviceChildren(theSession, theDevice, "block"); //check if the device has blocks in it
    std::vector<udev_device*> scsi_disk = getDeviceChildren(theSession, theDevice, "scsi_disk"); //check if the device has scsi_disks as children
    udev_device* usb = udev_device_get_parent_with_subsystem_devtype(theDevice, "usb", "usb_device"); //check if it is effectively connected via usb

    if( blocks.size() && scsi_disk.size() && usb ) return_value = true; //if has at least 1 block, an scsi disk and is connected via usb it is a disk, hopefully
    else return_value = false; //otherwise it is not

    //we need to manage the return value of getDeviceChildren
    for(unsigned int i = 0; i < blocks.size(); i++) udev_device_unref(blocks[i]);
    for(unsigned int i = 0; i < scsi_disk.size(); i++) udev_device_unref(scsi_disk[i]);
    udev_device_unref(usb);

    return return_value;
}

Result RemovableDisk::getConnectedDevices(std::vector<RemovableDisk>& theBuffer) {
    //clears the output parameter, just in case someone wants to be funny
    theBuffer.clear();

    //initializing a new udev session
    writeToLog("Initializing a new session for usb scanning...\n");
    udev* udev_session = udev_new();
    if( !udev_session ) return Result(ERR_SESSION_NOT_INITIALIZED);

    //find all th scsi devices (they are probably usb disks)
    udev_enumerate* enumerate = udev_enumerate_new(udev_session);
    udev_enumerate_add_match_subsystem(enumerate, "scsi");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "scsi_device");
    udev_enumerate_scan_devices(enumerate);

    udev_list_entry* entry;
    udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);

    //the actual scanning algorithm
    writeToLog("Scanning for usb devices...\n");
    udev_list_entry_foreach(entry, devices) {
        udev_device* dev = udev_device_new_from_syspath(udev_session, udev_list_entry_get_name(entry));

        if( isRemovableDisk(udev_session, dev) ) { //return them only if they are removable disks
            std::vector<udev_device*> blocks = getDeviceChildren(udev_session, dev, "block"); //find all the partitions in the disk
            for(unsigned int i = 0; i < blocks.size(); i++) {
                if( std::string(udev_device_get_devtype(blocks[i])) == "partition" ) theBuffer.push_back(RemovableDisk(udev_device_get_devnode(blocks[i]), udev_device_get_syspath(blocks[i]))); //add them only if they are partitions
                udev_device_unref(blocks[i]);
            }
        }
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev_session);
    return Result(Result::SUCCESS);
}

Result RemovableDisk::enableHotplugDetection() {
    if( hotplug_enabled ) return Result(Result::SUCCESS); //why even bother if already enabled?

    //initializing a new and separate udev session
    writeToLog("Initializing a new hotplug monitoring session...\n");
    monitor_session = udev_new();
    if( monitor_session == nullptr ) return Result(ERR_SESSION_NOT_INITIALIZED);

    //initializing the monitor needed for hotplug detection
    monitor = udev_monitor_new_from_netlink(monitor_session, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(monitor, "usb", "usb_device");
    udev_monitor_filter_add_match_subsystem_devtype(monitor, "block", NULL);
    udev_monitor_enable_receiving(monitor);

    //getting the file descriptor
    fd = udev_monitor_get_fd(monitor);

    //now that we are sure everything is ok we set the flag on
    hotplug_enabled = true;

    //starting the thread
    writeToLog("Starting the hotplug monitoring thread...\n");
    hotplug_thread = std::thread(&hotplugDetection);

    return Result(Result::SUCCESS);
}

Result RemovableDisk::disableHotplugDetection() {
    if( !hotplug_enabled ) return Result(Result::SUCCESS); //why even bother if it is already disabled

    //setting this to true will terminate the thread
    hotplug_exit = true;

    writeToLog("Waiting for the hotplug thread to terminate...\n");
    hotplug_thread.join(); //we wait for the thread to close

    //and we revert back all the control variables
    writeToLog("Disabling hotplug...\n");
    hotplug_exit = false;
    hotplug_enabled = false;
    udev_unref(monitor_session);
    monitor_session = nullptr;
    udev_monitor_unref(monitor);
    monitor = nullptr;
    hotplug_actions = std::queue<Action>(); //this is needed because we want to clear the action queue (and the queue doesn't support a clear function)
    fd = 0;
    return Result(Result::SUCCESS);
}

void RemovableDisk::hotplugDetection() {
    while( !hotplug_exit.load() ) {
        //set the fd to retrieve hotplug info in a blocking way (since this operation doesn't need to be so responsive we can even wait a full second
        fd_set fds;
        timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv); //did something happen?

        //if so let's see what happened
        udev_device* dev = udev_monitor_receive_device(monitor);
        if( ret > 0 && FD_ISSET(fd, &fds) ) {
            if( dev != nullptr ) { //a little bit of obnoxious checking (just in case something went wrong when retrieving the device
                if( strcmp(udev_device_get_devtype(dev), "partition") == 0 ) { //if the device is a disk
                    if( strcmp(udev_device_get_action(dev), "add") == 0 ) { //if something has been added
                        writeToLog("An hotplug event has been registered...\n");
                        hotplug_mutex.lock();
                        hotplug_actions.push(std::make_pair(RemovableDisk(udev_device_get_devnode(dev), udev_device_get_syspath(dev)), DEVICE_CONNECTED)); //if the device is connected we push the pair to the queue
                        hotplug_mutex.unlock();
                    }

                    else if( strcmp(udev_device_get_action(dev), "remove") == 0 ) { //if something has been removed
                        writeToLog("An hotplug event has been registered...\n");
                        hotplug_mutex.lock();
                        hotplug_actions.push(std::make_pair(RemovableDisk(udev_device_get_devnode(dev), udev_device_get_syspath(dev)), DEVICE_DISCONNECTED)); //the same if disconnected
                        hotplug_mutex.unlock();
                    }
                }
            }
        }
    }
}

std::vector<RemovableDisk::Action> RemovableDisk::getHotplugInformation() {
    std::vector<Action> to_return; //we will fill this vector with results and then we will return it
    if( !hotplug_enabled ) return to_return; //return immediately if hotplug is disabled (why even waste minimal cpu time?)

    hotplug_mutex.lock(); //CRITICAL SECTION
    while( !hotplug_actions.empty() ) {
        to_return.push_back(hotplug_actions.front()); //checking the queue with all the actions occured
        hotplug_actions.pop(); //popping the action away
    }
    hotplug_mutex.unlock(); //END OF CRITICAL SECTION

    return to_return;
}

std::string RemovableDisk::getMountInfo() const {
    //open mount table
    std::fstream stream;
    if( !FileSystem::openStream(stream, "/proc/mounts", FileSystem::INPUT) ) return "";

    std::string buffer;
    while( std::getline(stream, buffer) )
        if( buffer.compare(0, buffer.find(" "), device_path) == 0 ) //search for the device_path in the mount table
            return buffer;

    return "";
}

fs::path RemovableDisk::getMountPoint() const {
    //retrieve info from the mount table
    std::string mount_info;
    mount_info = getMountInfo();

    //if it is not present in the mounting table return an empty mount point
    if( mount_info.empty() ) return mount_info;

    else {
        //the mount point is after the first space and before the second
        std::size_t first_space_position = mount_info.find(" ");
        std::size_t second_space_position = mount_info.find(" ", first_space_position+1);

        //if, for some reason, we can't find any space we return an empty string
        if( first_space_position == std::string::npos || second_space_position == std::string::npos ) return "";

        //otherwise we return the correct substr
        else return mount_info.substr(first_space_position+1, second_space_position-first_space_position-1);
    }
}

Result RemovableDisk::mount() const {
    if( !isMounted() ) {
        writeToLog("Mounting "+device_path.string()+"...\n");
        std::string command = "pmount " + device_path.string();
        int system_result = system(command.c_str());
        if( system_result ) return Result(system_result);
        else return Result(Result::SUCCESS);
    }

    else return Result(Result::SUCCESS);
}

Result RemovableDisk::unmount() const {
    if( isMounted() ) {
        writeToLog("Unmounting "+device_path.string()+"...\n");
        std::string command = "pumount " + device_path.string();
        int system_result = system(command.c_str());
        if( system_result ) return Result(system_result);
        else return Result(Result::SUCCESS);
    }

    else return Result(Result::SUCCESS);
}

std::string RemovableDisk::getLabel() const {
    std::string buffer;

    udev* udev_session = udev_new();
    if( udev_session == nullptr ) return "";

    udev_device* device = udev_device_new_from_syspath(udev_session, sys_path.string().c_str());
    if( device == nullptr ) return "";

    //cycling through all the device attributes to see if we can find the label
    bool label_found = false;
    bool uuid_found = false; //if we don't find a label at least we try to read the fs_uuid
    udev_list_entry* list_buffer;
    udev_list_entry* list_first = udev_device_get_properties_list_entry(device);
    udev_list_entry_foreach(list_buffer, list_first) {
        if( strcmp(udev_list_entry_get_name(list_buffer), "ID_FS_LABEL") == 0 ) label_found = true;
        else if( strcmp(udev_list_entry_get_name(list_buffer), "ID_FS_UUID") == 0 ) uuid_found = true;

    }

    if( label_found ) buffer = udev_device_get_property_value(device, "ID_FS_LABEL");
    else if( uuid_found ) buffer = udev_device_get_property_value(device, "ID_FS_UUID");
    else buffer = "UNKNOWN"; //the remote case no label and no uuid is found

    udev_unref(udev_session);
    udev_device_unref(device);

    return buffer;
}
