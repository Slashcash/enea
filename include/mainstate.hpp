#ifndef _MAINSTATE_HPP_
#define _MAINSTATE_HPP_

#include <vector>
#include <map>

#include "state.hpp"
#include "sprite.hpp"
#include "cfgfile.hpp"
#include "sprite.hpp"
#include "rom.hpp"
#include "removabledisk.hpp"
#include "list.hpp"
#include "langfile.hpp"
#include "popup.hpp"
#include "menu.hpp"

class MainState : public State {
    private:
        //enums
        enum Section {
            TITLE_LIST,
            UPPER_MENU,
            POPUP,
            USB_MENU,
            ROM_MENU
        };

        //members
        CFGFile configuration_file; //the ENEA main configuration file
        std::vector<RemovableDisk> connected_disks; //all the connected usb disks
        std::vector<RemovableDisk> mounted_disks; //all the connected disks
        std::vector<Rom> hard_disk_roms;
        std::map<fs::path, std::vector<Rom >> usb_roms; //a map which contains a device and the roms on that device
        std::vector<Rom> all_roms; //a vector which conveniently stores all the name of the roms
        unsigned int rom_selected; //the rom which is currently selected
        unsigned int upper_menu_selected; //the upper menu entry which is currently selected
        LangFile language; //here we'll store all the language dependant strings
        Section active_section; //it stores which section of the gui is active right now
        Section backup_section; //a backup variable if we want to revert back to a previous section of the gui

        //graphical members
        Sprite background; //the main background
        List rom_list; //the rom list
        Row rom_title; //the rom infos
        Row rom_info;
        Row rom_year;
        Row rom_isclone;
        Row rom_clone;
        Sprite usb_sprite; //a little usb logo
        Row usb_info; //some on screen info for a rom which is on a usb key
        Sprite shutdown_button; //the shutdown button
        Sprite version_button;
        Sprite usblist_button;
        PopUp popup; //the popup we use to show some messages
        Menu menu; //a popup menu
        List menu_list; //the list that we show in the menu

        //functions
        virtual void updateEffectively();
        void drawEffectively() const;
        virtual void onNotify(const Signal& theSignal);
        void scanForRom();
        void buildRomList(); //this function conveniently builds the rom list on screen
        void buildRomInfo(); //it conveniently builds the selected rom information
        void buildRomMenu(); //builds the menu to show on a rom
        void buildUsbMenu(); //builds information about connected usb drives
        void showMenu(const Section& theSection); //calling this will show the menu on screen and will switch the section to theSection
        void hideMenu();
        void launchRom();
        void eraseRom();
        void copyRom();
        void showPopUp(const std::string& theMsg = ""); //it shows the popup with theMSG;
        void hidePopUp();
        void manageUpperMenu();

    public:
        //ctor
        MainState(sf::RenderWindow* theWindow);

        //dtor
        virtual ~MainState();
};

#endif
