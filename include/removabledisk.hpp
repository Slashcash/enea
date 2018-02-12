#ifndef _REMOVABLEDISK_HPP_
#define _REMOVABLEDISK_HPP_

#include <string>
#include <libudev.h>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

#include "result.hpp"
#include "logwriter.hpp"
#include "filesystem.hpp"

/*A class which abstracts the concept of a removable disk, you can't construct a removable disk on your own, but you can
have a list of already constructed device by checking for connected device with the provided functions, you also
have support for hotplug detection, you can mount and unmount disks. Please note that this class has a heavy reliance on
libudev*/

class RemovableDisk : public virtual LogWriter {
    public:
        //enum
        enum DeviceAction { //everything that can occur on a device
            DEVICE_CONNECTED,
            DEVICE_DISCONNECTED
        };

        //typedefs
        typedef std::pair<RemovableDisk, DeviceAction> Action; //an action is a pair which has a RemovableDisk and an action occured on that disk (you will get a vector of this when asking for hotplug detection)

    private:
        //members
        fs::path device_path; //the device path on the system
        fs::path sys_path;

        //static member
        static std::thread hotplug_thread; //this is the thread which will check for hotplugged devices (in separate thread because it is an expensive operation)
        static udev* monitor_session; //global variable needed for the hotplug detection
        static udev_monitor* monitor; //global variable needed for the hotplug detection
        static int fd; //global variable needed for the hotplug detection
        static bool hotplug_enabled; //a flag which will help us in detecting if the hotplug is already enabled
        static std::atomic<bool> hotplug_exit; //this variable is a flag which signals the hotplug_thread to stop when hotplug gets disabled
        static std::queue<Action> hotplug_actions; //the hotplug thread will store here all the occured actions ready to be read whenever is needed
        static std::mutex hotplug_mutex; //the mutex which will protect the queue from concurrent access

        //ctor
        RemovableDisk() = delete; //we don't want to build it this way
        RemovableDisk(const fs::path& theDevicePath, const fs::path& theSysPath);

        //functions
        std::string getMountInfo() const; //this function checks the mount table for the mount informations of this device

        //static functions
        static std::vector<udev_device*> getDeviceChildren(udev* theSession, udev_device* theParent, std::string theSubsystem); //this function gets all the children from a connected device (used for listing connected devices) NOTE: THE VECTOR HAS HEAP-ALLOCATED DEVICES, MANAGE THE POINTERS CORRECTLY ON YOUR OWN
        static bool isRemovableDisk(udev* theSession, udev_device* theDevice); //this functions ensures that a connected device really is a removable disk
        static void hotplugDetection(); //this function is meant to be called into a new thread, it fills the queue of new detected actions

    public:
        //enum
        enum Error {
            ERR_SESSION_NOT_INITIALIZED = 1,
        };

        //ctor
        RemovableDisk(const RemovableDisk& theDisk) = default; //i suggest not even this way, but hey, do what you want

        //getters
        fs::path getDevicePath() const { return device_path; } //returns the linux device path for this disk
        fs::path getSysPath() const { return sys_path; }
        fs::path getMountPoint() const;
        bool isMounted() const { return !getMountPoint().empty(); }

        //functions
        Result mount() const; //this function mounts this device to the specified point. It currently uses a pmount system call to ensure a mounting without sudo
        Result unmount() const; //same as mount
        std::string getLabel() const;

        //static functions
        static Result getConnectedDevices(std::vector<RemovableDisk>& theBuffer); //fills the buffer with all the connected devices
        static Result enableHotplugDetection();
        static Result disableHotplugDetection();
        static std::vector<Action> getHotplugInformation(); //this functions returns a vector of pair, every pair contains a device and the action occured on that device (ie: CONNECTED, DISCONNECTED)

        //operators
        RemovableDisk& operator=(const RemovableDisk& theDisk) = default; //what's the point of copying a removable disk anyway (from the public side)?
};

#endif // _REMOVABLEDISK_HPP_

