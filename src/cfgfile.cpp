#include "cfgfile.hpp"

#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

const std::string CFGFile::SEPARATOR = "=";
const fs::path CFGFile::DEFAULT_BASE_PATH = "share";
const fs::path CFGFile::BASE_PATH_STR = "base_path";
const fs::path CFGFile::ROM_PATH_STR = "rom_path";
const fs::path CFGFile::ART_PATH_STR = "art_path";
const fs::path CFGFile::FONTS_PATH_STR = "fonts_path";
const fs::path CFGFile::TEXTURES_PATH_STR = "textures_path";
const fs::path CFGFile::LANG_PATH_STR = "lang_path";
const fs::path CFGFile::SOUND_PATH_STR = "sounds_path";
const fs::path CFGFile::EMU_PATH_STR = "emu_path";

std::string CFGFile::getValue(const std::string& theKey) const {
    auto it = options_map.find(theKey);
    if( it == options_map.end() ) return "";
    else return it->second;
}

Result CFGFile::load(const fs::path& thePath) {
    options_map.clear();
    Result temp_res;

    writeToLog("Loading configuration file at "+thePath.string()+"...");

    std::fstream stream;
    FileSystem::openStream(stream, thePath, FileSystem::INPUT);

    if( !stream.is_open() ) {
        temp_res.setErrorNumber(LOAD_ERROR);
        temp_res.setDescription("Failed to open the file stream");
        writeToLog(temp_res, LogWriter::Type::ERROR);
        return temp_res;
    }

    std::string temp_buffer;
    while( std::getline(stream, temp_buffer) ) {
        std::size_t separator_position = temp_buffer.find(SEPARATOR);
        if( separator_position == std::string::npos ) {
            temp_res.setErrorNumber(PARSING_ERROR);
            temp_res.setDescription("Failed to parse the configuration file");
            return temp_res; //try to find the separator in the line
        }

        std::string key = temp_buffer.substr(0, separator_position); //separate the key and the value using the separator position
        std::string value = temp_buffer.substr(separator_position+1, std::string::npos);
        options_map.insert(std::make_pair(key, value));
    }

    return Result(Result::SUCCESS);
}

Result CFGFile::generateDefaultAndLoad(const fs::path& thePath) {
    writeToLog("Writing a default configuration file at "+thePath.string()+"...");

    std::ofstream stream;
    Result temp_res;
    stream.open(thePath);

    if( !stream.is_open() ) {
        temp_res.setErrorNumber(LOAD_ERROR);
        temp_res.setDescription("Failed to open the file stream");
        writeToLog(temp_res, LogWriter::Type::ERROR);
        return temp_res;
    }

    //getting the executable path
    fs::path executable_path;
    if( !FileSystem::getExecutablePath(executable_path) ) return Result(READLINK_ERROR);

    //write all default parameters
    stream << BASE_PATH_STR.string() << SEPARATOR << (executable_path / DEFAULT_BASE_PATH).string() << "\n";
    stream << ROM_PATH_STR.string() << SEPARATOR << "roms\n";
    stream << ART_PATH_STR.string() << SEPARATOR << "arts\n";
    stream << FONTS_PATH_STR.string() << SEPARATOR << "fonts\n";
    stream << TEXTURES_PATH_STR.string() << SEPARATOR << "textures\n";
    stream << LANG_PATH_STR.string() << SEPARATOR << "lang\n";
    stream << SOUND_PATH_STR.string() << SEPARATOR << "sounds\n";
    stream << EMU_PATH_STR.string() << SEPARATOR << "emulators\n";

    stream.close();
    return load(thePath);
}
