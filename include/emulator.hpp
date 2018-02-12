#ifndef _EMULATOR_HPP_
#define _EMULATOR_HPP_

#include <string>
#include <memory>
#include <miniz.h>

#include "result.hpp"
#include "filesystem.hpp"
#include "logwriter.hpp"
#include "database.hpp"

/*Specialize this pure virtual class to implement a new emulator for this frontend
URGENT TODO: CURRENTLY THE VALIDATEROM FUNCTION DOES NOT CHECK FOR THE EXISTENCE OF A DISK*/

class Emulator : public virtual LogWriter {
    private:
        //members
        fs::path emulator_path; //the path of the emulator executable
        std::string emulator_name; //the emulator name
        std::string emulator_romset; //the romset supported by this emulator

        //getters
        virtual std::vector<RomPart> getRomParts(const std::string& theRom) const final { if( emulator_database != nullptr ) return emulator_database->getRomParts(theRom); else return std::vector<RomPart>(); }
        virtual bool validatePartInArchive(mz_zip_archive& theArchive, const RomPart& thePart) const final;

        //pure virtual functions
        virtual Result runEffectively(const fs::path& theRomPath) const = 0;

    protected:
        //members
        std::shared_ptr<DataBase> emulator_database; //every emulator should have a rom database, right? (here because derivate may need easy access)

        //setters
        void setName(const std::string& theEmulatorName) { emulator_name = theEmulatorName; }
        void setRomset(const std::string& theEmulatorRomset) { emulator_romset = theEmulatorRomset; }

    public:
        //ctor
        Emulator() = delete; //do not construct an emulator this way
        Emulator(const fs::path& theEmulatorPath, const std::string& theEmulatorName, const std::string& theEmulatorRomset); //construct it this way

        //setters
        void setPath(const fs::path& theEmulatorPath) { emulator_path = theEmulatorPath; }

        //getters
        std::string getName() const { return emulator_name; }
        std::string getRomset() const { return emulator_romset; }
        fs::path getPath() const { return emulator_path; }
        virtual std::string getRomOrigin(const std::string& theRom) const final { if( emulator_database != nullptr ) return emulator_database->getRomOrigin(theRom); else return ""; }
        virtual bool isRomSupported(const std::string& theRom) const final { if( emulator_database != nullptr ) return emulator_database->isRomSupported(theRom); else return false; }
        virtual bool validateRom(const fs::path& theRomPath) const final;

        //functions
        virtual Result runRom(const fs::path& theRomPath) const final;
        virtual Result loadDB(const fs::path& theDBPath) final { if( emulator_database != nullptr ) return emulator_database->loadFromFile(theDBPath); else return Result(15); }

};

#endif // _EMULATOR_HPP_
