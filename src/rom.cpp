#include "rom.hpp"

#include <algorithm>

GeneralEmulator Rom::emulator;

Rom::Rom(const fs::path& theRomPath) {
    rom_paths.push_back(theRomPath);

    rom_name = theRomPath.stem().string();
    rom_filename = theRomPath.filename();
}

void Rom::merge(const Rom& theRom) {
    for( auto it = theRom.rom_paths.begin(); it < theRom.rom_paths.end(); it++ ) rom_paths.push_back(*it);
}

Result Rom::move(const fs::path& theNewLocation) const {
    Result temp_res;
    Result def_res;

    for( auto it = rom_paths.begin(); it < rom_paths.end(); it++ )
        if( !(temp_res = FileSystem::moveFile(*it, theNewLocation / rom_filename)) )
            def_res = temp_res; //even if it fails we don't return, we try the operation on the other paths too to see if something better happens

    return def_res;
}

Result Rom::copy(const fs::path& theNewLocation) const {
    Result temp_res;
    Result def_res;

    for( auto it = rom_paths.begin(); it < rom_paths.end(); it++ )
        if( !(temp_res = FileSystem::copyFile(*it, theNewLocation / rom_filename)) )
            def_res = temp_res;

    return def_res;
}

Result Rom::remove() const {
    Result temp_res;
    Result def_res;

    for( auto it = rom_paths.begin(); it < rom_paths.end(); it++ )
        if( !(temp_res = FileSystem::removeFile(*it)) )
            def_res = temp_res;

    return def_res;
}

std::vector<Rom> Rom::searchInDirectory(const fs::path& theDirectoryPath) {
    writeToLog("Scanning roms in "+theDirectoryPath.string()+"...\n");

    std::vector<Rom> buffer_vector;

    if( !fs::exists(theDirectoryPath) ) return buffer_vector;

    fs::recursive_directory_iterator it(theDirectoryPath);

    for( fs::begin(it); it != fs::end(it); it++ ) { //scanning all the directories
        if( fs::is_regular_file(it->path()) && it->path().extension() == ".zip" ) { //only adding regular files and .zip files to the vector
            Rom buffer(it->path());
            if( buffer.isRunnable() && buffer.isSupported() ) { //if the rom is not runnable (ie: a bios) we don't list it
                bool merged = false;
                for(auto it = buffer_vector.begin(); it < buffer_vector.end() && !merged; it++) if( it->getName() == buffer.getName() ) { it->merge(buffer); merged = true; } //if we find a copy of the same rom we merge it into the already existing one
                if( !merged ) buffer_vector.push_back(buffer); //otherwise we push it to the vector
            }
        }
    }

    std::sort(buffer_vector.begin(), buffer_vector.end()); //sorting the vector alphabetically using < operator
    return buffer_vector;
}

std::string Rom::getGame() const {
    std::string buffer = emulator.getRomName(rom_name);
    std::size_t position;
    if( (position = buffer.find("(")) == std::string::npos) return buffer;
    else return buffer.substr(0, position - 1);
}

std::string Rom::getInfo() const {
    std::string buffer = emulator.getRomName(rom_name);
    std::size_t position;
    if( (position = buffer.find("(")) == std::string::npos) return "";
    else return buffer.substr(position, std::string::npos);
}
