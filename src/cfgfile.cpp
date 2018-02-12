#include "cfgfile.hpp"

#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

const std::string CFGFile::SEPARATOR = "=";
const std::string CFGFile::DEFAULT_BASE_PATH = "share";
const std::string CFGFile::BASE_PATH_STR = "base_path";
const std::string CFGFile::ROM_PATH_STR = "rom_path";
const std::string CFGFile::ART_PATH_STR = "art_path";
const std::string CFGFile::FONTS_PATH_STR = "fonts_path";
const std::string CFGFile::TEXTURES_PATH_STR = "textures_path";
const std::string CFGFile::LANG_PATH_STR = "lang_path";
const std::string CFGFile::SOUND_PATH_STR = "sounds_path";
const std::string CFGFile::EMU_PATH_STR = "emu_path";

std::string CFGFile::getValue(const std::string& theKey) const {
    auto it = options_map.find(theKey);
    if( it == options_map.end() ) return "";
    else return it->second;
}

Result CFGFile::load(const fs::path& thePath) {
    options_map.clear();
    std::fstream stream;
    FileSystem::openStream(stream, thePath, FileSystem::INPUT);

    if( !stream.is_open() ) return Result(LOAD_ERROR);

    std::string temp_buffer;
    while( std::getline(stream, temp_buffer) ) {
        std::size_t separator_position = temp_buffer.find(SEPARATOR);
        if( separator_position == std::string::npos ) return Result(PARSING_ERROR); //try to find the separator in the line

        std::string key = temp_buffer.substr(0, separator_position); //separate the key and the value using the separator position
        std::string value = temp_buffer.substr(separator_position+1, std::string::npos);
        options_map.insert(std::make_pair(key, value));
    }


    return Result(Result::SUCCESS);
}

Result CFGFile::generateDefaultAndLoad(const fs::path& thePath) {
    std::ofstream stream;
    stream.open(thePath);

    if( !stream.is_open() ) return Result(LOAD_ERROR);

    //getting the executable path
    const int BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE];
    int readlink_result = readlink("/proc/self/exe", buffer, BUFFER_SIZE);
    if( readlink_result == -1 || readlink_result == BUFFER_SIZE ) return Result(READLINK_ERROR);
    buffer[readlink_result] = '\0';
    fs::path executable_path(buffer);
    executable_path.remove_filename();

    //write all default parameters
    stream << BASE_PATH_STR + SEPARATOR + executable_path.string() + DEFAULT_BASE_PATH << "\n";
    stream << ROM_PATH_STR + SEPARATOR + "/roms\n";
    stream << ART_PATH_STR + SEPARATOR + "/arts\n";
    stream << FONTS_PATH_STR + SEPARATOR + "/fonts\n";
    stream << TEXTURES_PATH_STR + SEPARATOR + "/textures\n";
    stream << LANG_PATH_STR + SEPARATOR + "/lang\n";
    stream << SOUND_PATH_STR + SEPARATOR + "/sounds\n";
    stream << EMU_PATH_STR + SEPARATOR + "/emulators\n";

    stream.close();
    return load(thePath);
}
