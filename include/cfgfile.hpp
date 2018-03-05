#ifndef _CFGFILE_HPP_
#define _CFGFILE_HPP_

#include <map>
#include <string>

#include "result.hpp"
#include "filesystem.hpp"
#include "logwriter.hpp"

/*This class abstracts the concept of a configuration file for the core part of this software
the cfg file has entry built like this: field=value
the base path is the base folder for this program and every other path is relative to this*/

class CFGFile : virtual public LogWriter {
    private:
        //const
        static const std::string SEPARATOR; //The separator between field and value
        static const fs::path DEFAULT_BASE_PATH; //the default base path for this software
        static const fs::path BASE_PATH_STR; //the string which stores the FIELD for base path
        static const fs::path ROM_PATH_STR; //the string which stores the FIELD for rom
        static const fs::path ART_PATH_STR; //the string which stores the FIELD for art
        static const fs::path FONTS_PATH_STR; //the string which stores the FIELD for fonts
        static const fs::path TEXTURES_PATH_STR; //the string which stores the FILED for textures
        static const fs::path LANG_PATH_STR; //the string which stores the FIELD value for language packs
        static const fs::path SOUND_PATH_STR; //the string which stores the FIELD value for sounds
        static const fs::path EMU_PATH_STR; //the string which stores the FIELD value for the emulators location

        //members
        std::map<const std::string, std::string> options_map; //the map which stores FIELD and VALUE pairs

        //functions
        std::string getValue(const std::string& theKey) const; //this function explores the map

    public:
        //enums
        enum Error { //a lot of things can go wrong
            LOAD_ERROR = 1,
            READLINK_ERROR,
            PARSING_ERROR,
            WRITE_ERROR
        };

        //ctor
        CFGFile() = default;

        //functions
        //using this get* functions to retrieve the different parameters
        fs::path getDefaultBasePath() const { return DEFAULT_BASE_PATH; }
        fs::path getEmulatorsPath() const { return getBasePath() / fs::path(getValue(EMU_PATH_STR)); }
        fs::path getRomPath() const { return getBasePath() / getRomFolder(); }
        fs::path getArtPath() const { return getValue(BASE_PATH_STR) / getArtFolder(); }
        fs::path getSoundPath() const { return getValue(BASE_PATH_STR) / getValue(SOUND_PATH_STR); }
        fs::path getFontsPath() const { return getValue(BASE_PATH_STR) / getValue(FONTS_PATH_STR); }
        fs::path getTexturesPath() const { return getValue(BASE_PATH_STR) / getValue(TEXTURES_PATH_STR); }
        fs::path getRomFolder() const { return getValue(ROM_PATH_STR); }
        fs::path getArtFolder() const { return getValue(ART_PATH_STR); }
        fs::path getLangPath() const { return getValue(BASE_PATH_STR) / getValue(LANG_PATH_STR); }
        fs::path getBasePath() const { return getValue(BASE_PATH_STR); }
        Result load(const fs::path& thePath); //loads from disk
        Result generateDefaultAndLoad(const fs::path& thePath); //it writes to disk a default configuration file and then loads it
};

#endif // _CFGFILE_HPP_
