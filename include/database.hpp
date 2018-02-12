#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include "filesystem.hpp"
#include "logwriter.hpp"
#include "rompart.hpp"

class DataBase : public virtual LogWriter {
    private:
        //pure virtual functions
        virtual Result loadFromFileEffectively(const fs::path& theDBPath) = 0;

    public:
        //ctor
        DataBase() = default;
        DataBase(const DataBase& theDB) = delete; //seriously don't copy a db, it can be so memory intensive

        //functions
        virtual Result loadFromFile(const fs::path& theDBPath) final { writeToLog("Loading database at "+theDBPath.string()+"...\n"); return loadFromFileEffectively(theDBPath); };

        //pure virtual functions
        virtual std::string getRomOrigin(const std::string& theRom) const = 0; //this function should return the rom which may be merged with this (ie: neogeo when launching mslug)
        virtual bool isRomSupported(const std::string& theRom) const = 0;
        virtual std::vector<RomPart> getRomParts(const std::string& theRomName) const = 0;

        //operators
        DataBase& operator=(const DataBase& theDB) = delete; //as above
};

#endif
