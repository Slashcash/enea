#ifndef _ROM_HPP_
#define _ROM_HPP_

#include <vector>

#include "filesystem.hpp"
#include "generalemulator.hpp"
#include "logwriter.hpp"

/*this file abstracts the concept of a rom, a lot of cool tricks can be called on this
NOTE: LOAD THE SHARED ROM DATABASE BEFORE DOING ANYTHING ELSE ON THIS CLASS, IT HAS TO BE CALLED EXPLICITLY BECAUSE IT IS AN HEAVY AND TIME CONSUMING OPERATION*/

class Rom : virtual public LogWriter {
    private:
        //members
        std::vector<fs::path> rom_paths; //the rom path (it is a vector of multiple paths because the same rom can be on different path of the filesystem and we merge everything into a single one
        std::string rom_name;
        fs::path rom_filename;

        //static members
        static GeneralEmulator emulator; //every rom share the same emulator

        //functions
        bool isSupported() const { return emulator.isRomSupported(rom_name); }
        bool isRunnable() const { return emulator.isRomRunnable(rom_name); }

    public:
        //ctor
        Rom() = delete;
        Rom(const fs::path& theRomPath);

        //getters
        std::string getName() const { return rom_name; }
        std::string getFileName() const { return rom_filename; }
        std::string getGame() const;
        std::string getInfo() const;
        std::string getYear() const { return emulator.getRomYear(rom_name); }
        std::string getManufacturer() const { return emulator.getRomManufacturer(rom_name); }
        std::string getParent() const { return emulator.getRomParent(rom_name); }
        std::vector<fs::path> getPaths() const { return rom_paths; }

        //functions
        Result launch() const { return emulator.runRom(rom_paths); }
        Result checkLaunchability() const { return emulator.checkRomLaunchability(rom_paths); }
        void merge(const Rom& theRom); //what does this means? if we find the same rom in different paths since they are the same we merge them in the same structure (so when we launch it the emulator can choose the best one to use among them)
        Result move(const fs::path& theNewLocation) const;
        Result remove() const;
        Result copy(const fs::path& theNewLocation) const;

        //static functions
        static Result loadRomDB(const fs::path& theEmulatorFolder) { emulator = GeneralEmulator(theEmulatorFolder); return emulator.loadDB(); }
        static std::vector<Rom> searchInDirectory(const fs::path& theDirectoryPath);

        //operators
        bool operator<(const Rom& theRom) { return this->getGame() < theRom.getGame(); } //it is used to sort the roms alphabetically with the sort algorithm

};

#endif // _ROM_HPP_
