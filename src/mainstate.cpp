#ifdef NDEBUG
    #define VERSION "DEBUG"
#else
    #define VERSION "0.1.1 ALPHA"
#endif

#include "mainstate.hpp"

#include "errorstate.hpp"

MainState::MainState(sf::RenderWindow* theWindow) : State(theWindow) {
    active_section = TITLE_LIST;
    backup_section = TITLE_LIST;
    upper_menu_selected = 1; //by default the upper menu has the shut down button selected

    Result temp_res;

    //loading the configuration file
    fs::path executable_path;
    FileSystem::getExecutablePath(executable_path);
    if( !configuration_file.load(executable_path / configuration_file.getDefaultBasePath() / "enea.cfg") )  //if it fails generate a default one
        if( !(temp_res = configuration_file.generateDefaultAndLoad(configuration_file.getDefaultBasePath() / "enea.cfg")) ) {
            exit();
            temp_res.setDescription("CFG_FILE");
            requestStateChange(new ErrorState(window, temp_res));
        }

    //loading the rom database
    if( !(temp_res = Rom::loadRomDB(configuration_file.getEmulatorsPath())) ) {
        exit();
        temp_res.setDescription("ROM_DB_FILE");
        requestStateChange(new ErrorState(window, temp_res));
    }

    //loading the language file
    if( !(temp_res = language.load(configuration_file.getLangPath() / "eng.lang")) ) {
        exit();
        temp_res.setDescription("LANG_FILE");
        requestStateChange(new ErrorState(window, temp_res));
    }

    //enabling usb hotplugging detection
    if( !(temp_res = RemovableDisk::enableHotplugDetection()) ) {
        exit();
        temp_res.setDescription("USB_HOTPLUG");
        requestStateChange(new ErrorState(window, temp_res));
    }

    //checking for connected usb devices
    if( !(temp_res = RemovableDisk::getConnectedDevices(connected_disks)) ) {
        exit();
        temp_res.setDescription("USB_DETECTION");
        requestStateChange(new ErrorState(window, temp_res));
    }

    //mounting all the connected devices
    for( auto it = connected_disks.begin(); it < connected_disks.end(); it++ ) {
        if( !it->isMounted() ) it->mount();
        if( it->isMounted() ) mounted_disks.push_back(*it);
    }

    //scanning
    scanForRom();

    //setting up the background
    background.setTexture(texture_manager.getTexture(configuration_file.getTexturesPath() / "background.png"));
    scene.attachToLayer(&background, 0);

    //setting up the upper menu
    shutdown_button.setTexture(texture_manager.getTexture(configuration_file.getTexturesPath() / "shutoff.png"));
    shutdown_button.setOrigin(shutdown_button.getLocalBounds().width/2, shutdown_button.getLocalBounds().height/2);
    shutdown_button.setPosition(scene.getSize().x/2, 100);
    scene.attachToLayer(&shutdown_button, 1);

    version_button.setTexture(texture_manager.getTexture(configuration_file.getTexturesPath() / "version.png"));
    version_button.setOrigin(version_button.getLocalBounds().width/2, version_button.getLocalBounds().height/2);
    version_button.setPosition(shutdown_button.getGlobalPosition().x+shutdown_button.getLocalBounds().width+version_button.getLocalBounds().width/2+80, shutdown_button.getGlobalPosition().y+version_button.getLocalBounds().height/2);
    scene.attachToLayer(&version_button, 1);

    usblist_button.setTexture(texture_manager.getTexture(configuration_file.getTexturesPath() / "usb_list.png"));
    usblist_button.setOrigin(usblist_button.getLocalBounds().width/2, usblist_button.getLocalBounds().height/2);
    usblist_button.setPosition(shutdown_button.getGlobalPosition().x-usblist_button.getLocalBounds().width/2-80, shutdown_button.getGlobalPosition().y+usblist_button.getLocalBounds().height/2);
    scene.attachToLayer(&usblist_button, 1);

    //building the rom list
    rom_list.setPosition(100, 200);
    rom_list.setTruncateWidth(350);
    rom_list.setUpdateRate(sf::seconds(0.3));
    rom_list.setTruncateHeight(700);
    rom_list.setHighLightColor(sf::Color::Red);
    buildRomList();
    scene.attachToLayer(&rom_list, 1);

    //building the rom info
    rom_title.attachChild(&rom_info);
    rom_title.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    rom_title.setSize(40);
    rom_info.attachChild(&rom_year);
    rom_info.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    rom_info.setSize(40);
    rom_year.attachChild(&rom_isclone);
    rom_year.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    rom_year.setSize(40);
    rom_isclone.attachChild(&rom_clone);
    rom_isclone.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    rom_isclone.setSize(25);
    rom_clone.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    rom_clone.setSize(25);
    usb_sprite.setTexture(texture_manager.getTexture(configuration_file.getTexturesPath() / "usb.png"));
    usb_sprite.attachChild(&usb_info);
    usb_sprite.setPosition(rom_list.getGlobalPosition().x, 1000);
    usb_info.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    usb_info.setSize(25);
    usb_info.setPosition(usb_sprite.getLocalBounds().width + 15, 0);
    buildRomInfo();
    scene.attachToLayer(&rom_title, 1);

    //building the popup
    popup.setTexture(texture_manager.getTexture(configuration_file.getTexturesPath() / "popup.png"));
    popup.setOrigin(popup.getLocalBounds().width/2, popup.getLocalBounds().height/2);
    popup.setPosition(scene.getSize().x/2, scene.getSize().y/2);
    popup.setUpdateRate(sf::seconds(0.2));

    //building the menu
    menu.setOrigin(menu.getLocalBounds().width/2, menu.getLocalBounds().height/2);
    menu.setPosition(scene.getSize().x/2, scene.getSize().y/2);

    menu_list.setHighLightColor(sf::Color::Red);
    menu_list.setMainColor(sf::Color::Black);
    menu_list.setUpdateRate(sf::seconds(0.2));

    //managing sounds
    std::vector<SoundCommand> sound_buffer;
    sound_buffer.push_back(std::make_pair(Signal("ROM_SELECTION"), configuration_file.getSoundPath() / "selection.wav"));
    sound_buffer.push_back(std::make_pair(Signal("UPPER_MENU_SELECTION"), configuration_file.getSoundPath() / "selection.wav"));
    sound_buffer.push_back(std::make_pair(Signal("MENU_SELECTION"), configuration_file.getSoundPath() / "selection.wav"));
    sound_buffer.push_back(std::make_pair(Signal("ROM_LAUNCH"), configuration_file.getSoundPath() / "launch.wav"));
    sound_buffer.push_back(std::make_pair(Signal("USB_EVENT"), configuration_file.getSoundPath() / "usb.wav"));

    sound_manager.setSoundCommand(sound_buffer);
}

void MainState::updateEffectively() {
    //manage usb hotplug
    std::vector<RemovableDisk::Action> action_buffer = RemovableDisk::getHotplugInformation();

    for( auto it = action_buffer.begin(); it < action_buffer.end(); it++ ) {
        if( it->second == RemovableDisk::DeviceAction::DEVICE_CONNECTED ) { //USB CONNECTION
            if( !it->first.isMounted() ) it->first.mount();
            connected_disks.push_back(it->first); //add it to the connected disks
            if( it->first.isMounted() ) mounted_disks.push_back(it->first); //if is mounted add it to the mounted disks
            std::vector<Rom> usb_buffer = Rom::searchInDirectory(it->first.getMountPoint() / configuration_file.getRomFolder());
            if( !usb_buffer.empty() ) {usb_roms.insert(std::make_pair(it->first.getDevicePath(), usb_buffer)); buildRomList(); buildRomInfo(); }

            if( active_section == USB_MENU ) { hideMenu(); buildUsbMenu(); } //if we are showing connected devices and one of them gets disconnected we wanto to update the list
            else if( active_section == ROM_MENU ) hideMenu();

            emitSignal(Signal("USB_EVENT"));
        }

        else if( it->second == RemovableDisk::DeviceAction::DEVICE_DISCONNECTED ) { //USB DISCONNECTION
            if( it->first.isMounted() ) it->first.unmount();
            for( auto it2 = connected_disks.begin(); it2 < connected_disks.end(); it2++) if( it2->getDevicePath() == it->first.getDevicePath() ) connected_disks.erase(it2); //erasing it from the connected disks
            for( auto it2 = mounted_disks.begin(); it2 < mounted_disks.end(); it2++) if( it2->getDevicePath() == it->first.getDevicePath() ) mounted_disks.erase(it2); //erasing it from the mounted disks
            auto found = usb_roms.find(it->first.getDevicePath());
            if( found != usb_roms.end() ) { usb_roms.erase(found); buildRomList(); buildRomInfo(); }

            if( active_section == USB_MENU ) { hideMenu(); buildUsbMenu(); }
            else if( active_section == ROM_MENU ) hideMenu();

            emitSignal(Signal("USB_EVENT"));
        }
    }
}

void MainState::onNotify(const Signal& theSignal) {
    if( theSignal.getName() == "INPUT_EXIT" ) exit();

    if( theSignal.getName() == "INPUT_P1DOWN" ) {
        if( rom_selected < all_roms.size()-1 && active_section == TITLE_LIST ) { rom_selected++; rom_list.setSelected(rom_selected); buildRomInfo(); emitSignal(Signal("ROM_SELECTION")); }
        else if( (active_section == USB_MENU || active_section == ROM_MENU ) && menu.getSelected() < menu_list.getElementNumber() -1 ) { menu.setSelected(menu.getSelected()+1); emitSignal(Signal("MENU_SELECTION")); }
    }

    if( theSignal.getName() == "INPUT_P1UP" ) {
        if( rom_selected > 0 && active_section == TITLE_LIST ) { rom_selected--; rom_list.setSelected(rom_selected); buildRomInfo(); emitSignal(Signal("ROM_SELECTION")); }
        else if( (active_section == USB_MENU || active_section == ROM_MENU ) && menu.getSelected() > 0 ) { menu.setSelected(menu.getSelected()-1); emitSignal(Signal("MENU_SELECTION")); }
    }

    if( theSignal.getName() == "INPUT_P1RIGHT" ) {
        if( active_section == UPPER_MENU && upper_menu_selected < 2) { upper_menu_selected++; manageUpperMenu(); emitSignal(Signal("UPPER_MENU_SELECTION")); }
    }

    if( theSignal.getName() == "INPUT_P1LEFT" ) {
        if( active_section == UPPER_MENU && upper_menu_selected > 0) { upper_menu_selected--; manageUpperMenu(); emitSignal(Signal("UPPER_MENU_SELECTION")); }

    }

    if( theSignal.getName() == "INPUT_P1START" ) {
        if( active_section == TITLE_LIST ) launchRom();
        else if( active_section == UPPER_MENU && upper_menu_selected == 1 ) system("shutdown -h now");
        else if( active_section == UPPER_MENU && upper_menu_selected == 2 ) showPopUp(VERSION);
        else if( active_section == UPPER_MENU && upper_menu_selected == 0 ) buildUsbMenu();
        else if( active_section == POPUP ) hidePopUp();
        else if( active_section == USB_MENU ) {
            Result unmount_result = mounted_disks[menu.getSelected()].unmount();
            hideMenu();
            if(!unmount_result) showPopUp(language.getValue("unmount_failed"));
            else {
                mounted_disks.erase(mounted_disks.begin() + menu.getSelected());
                auto found = usb_roms.find(connected_disks[menu.getSelected()].getDevicePath());
                if( found != usb_roms.end() ) { usb_roms.erase(found); buildRomList(); }
            }
        }
        else if( active_section == ROM_MENU ) {
            hideMenu();

            if( menu.getSelected() == 0 ) launchRom();
            else if( menu.getSelected() == 1 ) eraseRom();
            else if( menu.getSelected() == 2 ) copyRom();

        }
    }

    if( theSignal.getName() == "INPUT_P1COIN" ) {
        if( active_section == TITLE_LIST ) {
            rom_selected = 0;
            rom_list.setSelected(0);
            buildRomInfo();
            active_section = UPPER_MENU;
            rom_list.setActive(false);
            manageUpperMenu();
        }

        else if( active_section == UPPER_MENU ) {
            active_section = TITLE_LIST;
            rom_list.setActive(true);
            upper_menu_selected = 1;
            shutdown_button.setScale(1, 1);
            version_button.setScale(1, 1);
            usblist_button.setScale(1, 1);
        }

        else if( active_section == USB_MENU || active_section == ROM_MENU ) hideMenu();
    }

    if( theSignal.getName() == "INPUT_P1BUTTON1" ) {
        if( active_section == UPPER_MENU && upper_menu_selected == 1) system("shutdown -h now");
        else if( active_section == UPPER_MENU && upper_menu_selected == 2 ) showPopUp(VERSION);
        else if( active_section == UPPER_MENU && upper_menu_selected == 0 ) buildUsbMenu();
        else if( active_section == POPUP ) hidePopUp();
        else if( active_section == TITLE_LIST ) buildRomMenu();
        else if( active_section == USB_MENU ) {
            Result unmount_result = connected_disks[menu.getSelected()].unmount();
            hideMenu();
            if(!unmount_result) showPopUp(language.getValue("unmount_failed"));
            else {
                mounted_disks.erase(mounted_disks.begin() + menu.getSelected());
                auto found = usb_roms.find(connected_disks[menu.getSelected()].getDevicePath());
                if( found != usb_roms.end() ) { usb_roms.erase(found); buildRomList(); }
            }
        }
        else if( active_section == ROM_MENU ) {
            hideMenu();

            if( menu.getSelected() == 0 ) launchRom();
            else if( menu.getSelected() == 1 ) eraseRom();
            else if( menu.getSelected() == 2 ) copyRom();

        }
    }
}

void MainState::scanForRom() {
    //scanning the hard disk
    hard_disk_roms = Rom::searchInDirectory(configuration_file.getRomPath());

    //scanning usb
    for( auto it = mounted_disks.begin(); it < mounted_disks.end(); it++ ) {
        std::vector<Rom> rom_buffer = Rom::searchInDirectory(it->getMountPoint() / configuration_file.getRomFolder());
        if( !rom_buffer.empty() ) usb_roms.insert(std::make_pair(it->getDevicePath(), rom_buffer));
    }
}

void MainState::buildRomList() {
    rom_selected = 0;
    rom_list.setSelected(0);
    all_roms.clear();


    for(auto it = hard_disk_roms.begin(); it < hard_disk_roms.end(); it++) all_roms.push_back(*it);
    for(auto it = usb_roms.begin(); it != usb_roms.end(); it++) for( auto it2 = it->second.begin(); it2 < it->second.end(); it2++ ) { all_roms.push_back(*it2); }
    std::sort(all_roms.begin(), all_roms.end());

    std::vector<Row> buffer;
    //if( !all_roms.empty() ) {

        for(auto it = all_roms.begin(); it < all_roms.end(); it++) {
            Row row_buffer;
            row_buffer.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
            row_buffer.setString(it->getGame());
            buffer.push_back(row_buffer);
        }
    //}

    rom_list.setRowVector(buffer);
}

void MainState::buildRomInfo() {
    if( !all_roms.empty() ) { //if no rom is found we don't show any info
        const float SPACING = 15;

        rom_title.setString(all_roms[rom_selected].getGame());
        rom_title.setOrigin(rom_title.getLocalBounds().width/2, 0);
        rom_title.setPosition(scene.getSize().x/2, 450);

        rom_info.setString(all_roms[rom_selected].getInfo());
        rom_info.setOrigin(rom_info.getLocalBounds().width/2, 0);
        rom_info.setPosition(rom_title.getLocalBounds().width/2, rom_title.getLocalBounds().height+SPACING);

        rom_year.setString(all_roms[rom_selected].getManufacturer()+", "+all_roms[rom_selected].getYear());
        rom_year.setOrigin(rom_year.getLocalBounds().width/2, 0);
        rom_year.setPosition(rom_info.getLocalBounds().width/2, rom_info.getLocalBounds().height+SPACING);

        std::string rom_parent;
        if( !(rom_parent = all_roms[rom_selected].getParent()).empty() ) {
            rom_isclone.setString(language.getValue("cloneof"));
            rom_isclone.setOrigin(rom_isclone.getLocalBounds().width/2, 0);
            rom_isclone.setPosition(rom_year.getLocalBounds().width/2, rom_year.getLocalBounds().height+SPACING*3);

            rom_clone.setString(rom_parent);
            rom_clone.setOrigin(rom_clone.getLocalBounds().width/2, 0);
            rom_clone.setPosition(rom_isclone.getLocalBounds().width/2, rom_isclone.getLocalBounds().height+SPACING);
        }

        else { rom_isclone.setString(""); rom_clone.setString(""); }

        //we check if the rom is on hdd or on usb
        std::string temp_rom_path;

        if( (temp_rom_path = all_roms[rom_selected].getPaths()[0].string()).find(configuration_file.getRomPath()) == 0 ) scene.detachFromLayer(&usb_sprite, 1); //if it is not on an usb stick we don't show information on which stick contains the rom
        else { //which stick contains it?
            for(auto it = connected_disks.begin(); it < connected_disks.end(); it++)
                if( temp_rom_path.find(it->getMountPoint()) == 0 )
                    usb_info.setString(it->getLabel());

            scene.attachToLayer(&usb_sprite, 1);
        }
    }

    else { //We show a message instead and delete all the other info
        rom_title.setString(language.getValue("no_rom_found"));
        rom_title.setOrigin(rom_title.getLocalBounds().width/2, 0);
        rom_title.setPosition(scene.getSize().x/2, 450);

        rom_info.setString("");
        rom_year.setString("");
        rom_isclone.setString("");
        rom_clone.setString("");
        scene.detachFromLayer(&usb_sprite, 1);
    }
}

void MainState::buildRomMenu() {
    std::vector<Row> buffer;

    Row row1;
    row1.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    row1.setString(language.getValue("launch_rom"));
    buffer.push_back(row1);

    Row row2;
    row2.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
    row2.setString(language.getValue("erase_rom"));
    buffer.push_back(row2);


    if( all_roms[rom_selected].getPaths()[0].string().find(configuration_file.getRomPath()) != 0 ) { //if it is on hdd
        Row row3;
        row3.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
        row3.setString(language.getValue("copy_rom_hdd"));
        buffer.push_back(row3);
    }

    menu_list.setRowVector(buffer);
    showMenu(ROM_MENU);

}

void MainState::buildUsbMenu() {
    if( !mounted_disks.empty() ) {
        std::vector<Row> buffer;

        for(auto it = mounted_disks.begin(); it < mounted_disks.end(); it++) {
            Row row_buffer;
            row_buffer.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
            row_buffer.setString(it->getLabel());

            buffer.push_back(row_buffer);
        }

        menu_list.setRowVector(buffer);
        showMenu(USB_MENU);
    }

    else showPopUp(language.getValue("no_disks"));
}

void MainState::showPopUp(const std::string& theMsg) {
    backup_section = active_section;
    active_section = POPUP;

    if( !theMsg.empty() ) {
        Row temp;
        temp.setColor(sf::Color::Red);
        temp.setFont(font_manager.getFont(configuration_file.getFontsPath() / "dtmsans.otf"));
        temp.setString(theMsg);
        popup.setRow(temp);
    }

    scene.attachToLayer(&popup, 2);
}

void MainState::showMenu(const Section& theSection) {
    backup_section = active_section;
    active_section = theSection;

    menu.setList(menu_list);
    scene.attachToLayer(&menu, 2);
}

void MainState::hideMenu() {
    active_section = backup_section;

    scene.detachFromLayer(&menu, 2);
}

void MainState::hidePopUp() {
    active_section = backup_section;
    scene.detachFromLayer(&popup, 2);
}

void MainState::manageUpperMenu() {
    usblist_button.setScale(1, 1);
    shutdown_button.setScale(1, 1);
    version_button.setScale(1, 1);


    if( upper_menu_selected == 0 ) usblist_button.setScale(1.5, 1.5);
    if( upper_menu_selected == 1 ) shutdown_button.setScale(1.5, 1.5);
    if( upper_menu_selected == 2 ) version_button.setScale(1.5, 1.5);
}

void MainState::launchRom() {
    emitSignal(Signal("ROM_LAUNCH"));
    Result launch_result;

    //in case launching the rom goes wrong in any way
    if( !(launch_result = all_roms[rom_selected].checkLaunchability()) ) {
        if( launch_result.getErrorNumber() == GeneralEmulator::ERR_ROM_NOT_SUPPORTED || launch_result.getErrorNumber() == GeneralEmulator::ERR_ROM_NOT_VALID ) showPopUp(language.getValue("rom_not_valid"));
        if( launch_result.getErrorNumber() == GeneralEmulator::ERR_ROM_NOT_ACCESSIBLE ) showPopUp(language.getValue("rom_not_accessible"));
        if( launch_result.getErrorNumber() == GeneralEmulator::ERR_ROM_NOT_RUNNABLE ) showPopUp(language.getValue("rom_not_runnable"));
        if( launch_result.getErrorNumber() == GeneralEmulator::ERR_INVALID_VECTOR ) showPopUp(language.getValue("rom_unknown"));
    }

    else {
        window->close();
        if( all_roms[rom_selected].launch() == GeneralEmulator::ERR_EMULATOR) showPopUp(language.getValue("rom_emulator"));
        //OH MY GOD WHY THIS SHIT IS HERE IT BURNS MY EYES (while waiting for an elegant solution since i'm bored to death this will stay hardcoded here
        window->create(sf::VideoMode(1920, 1080), "Enea", sf::Style::Fullscreen); //creating the actual window
        window->setFramerateLimit(25); //25 frames are really more than enough (we could switch this to 20 so this won't save some precious electrical power)
        window->setMouseCursorVisible(false);
    }
}

void MainState::eraseRom() {
    std::string popup_str;

    if( all_roms[rom_selected].remove() ) {
        popup_str = language.getValue("erase_rom_success");
        scanForRom();
        buildRomList();
    }

    else popup_str = language.getValue("erase_rom_failed");

    buildRomInfo();
    showPopUp(popup_str);
}

void MainState::copyRom() {
    std::string popup_str;

    if( all_roms[rom_selected].copy(configuration_file.getRomPath()) ) {
        popup_str = language.getValue("copy_rom_success");
        scanForRom();
        buildRomList();
    }

    else popup_str = language.getValue("copy_rom_failed");

    showPopUp(popup_str);
}

MainState::~MainState() {
    RemovableDisk::disableHotplugDetection();
}
