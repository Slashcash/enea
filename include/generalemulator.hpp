#ifndef _GENERALEMULATOR_HPP_
#define _GENERALEMULATOR_HPP_

#include <vector>
#include <memory>

#include "logwriter.hpp"
#include "emulator.hpp"
#include "xmldatabase.hpp"

/*as this frontend may support a wide varierty of emulators this class implements a generalemulator, a concept which provides
the abstraction of a single emulator for upper level of this software. Upper level classes should just run rom using this fake emulator
and it will take care of choosing the perfect emulator for the selected rom, it will provide information on roms accessing every emulator database
and a lot of cool stuffs*/

class GeneralEmulator : public virtual LogWriter {
    private:
        //typedefs
        typedef std::pair<std::shared_ptr<Emulator>, fs::path> emulator_pair; //this pair is used in the chooseEmulator function

        //static members
        static XMLDataBase general_database; //this represents the database which is shared between all emulators

        //members
        std::vector<std::shared_ptr<Emulator>> supported_emulators; //this vector contains every supported emulator
        fs::path emulators_folder; //the folder which contains every emulator

        //functions
        emulator_pair chooseEmulator(const std::vector<fs::path>& theRomsPath, const fs::path& theBasePath) const; //this functions takes a series of rom paths (representing same roms in different paths) and chooses which one of the supported emulators is best suited to run it

    public:
        //enum
        enum Error {
            ERR_ROM_NOT_SUPPORTED = 1,
            ERR_INVALID_VECTOR,
            ERR_ROM_NOT_RUNNABLE,
            ERR_ROM_NOT_VALID,
            ERR_ROM_NOT_ACCESSIBLE,
            ERR_EMULATOR
        };

        //ctor
        GeneralEmulator() = default;
        GeneralEmulator(const fs::path& theEmulatorsFolder); //construct a general emulator by specifying the folder which contains informations about all emulators

        //getters
        fs::path getEmulatorFolder() const { return emulators_folder; }

        //setters
        void setEmulatorFolder(const fs::path& theEmulatorFolder) { emulators_folder = theEmulatorFolder; }

        //functions
        Result loadDB(); //use this function to load the rom database
        std::string getRomName(const std::string& theRom) const { return general_database.getRomName(theRom); }
        std::string getRomYear(const std::string& theRom) const { return general_database.getRomYear(theRom); }
        std::string getRomManufacturer(const std::string& theRom) const { return general_database.getRomManufacturer(theRom); }
        std::string getRomParent(const std::string& theRom) const { return general_database.getRomParent(theRom); }
        bool isRomSupported(const std::string& theRom) const { return general_database.isRomSupported(theRom); }
        bool isRomRunnable(const std::string& theRom) const { return general_database.isRomRunnable(theRom); }
        Result checkRomLaunchability(const std::vector<fs::path>& theRomsPath, const fs::path& theBasePath) const; //this checks if the rom is launchable but doesn't launch it
        Result runRom(const std::vector<fs::path>& theRomsPath, const fs::path& theBasePath) const; //this checks and launches it instead, see emulator.hpp for information about theBasePath
};

#endif // _GENERALEMULATOR_HPP_
