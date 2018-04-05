#include "generalemulator.hpp"

#include <algorithm>

#include "advmame.hpp"

XMLDataBase GeneralEmulator::general_database;

GeneralEmulator::GeneralEmulator(const fs::path& theEmulatorsFolder) {
    emulators_folder = theEmulatorsFolder;

    //adding advmame to supported emulators
    supported_emulators.push_back(std::make_shared<AdvMAME>(theEmulatorsFolder / "AdvMAME" / "AdvMAME"));
}

Result GeneralEmulator::loadDB() {
    Result temp_res;

    if( !(temp_res = general_database.loadFromFile(emulators_folder / "db.dat")) ) { temp_res.setDescription("general"); return temp_res; }

    //iterating through every supported emulator to load its db
    for(auto it = supported_emulators.begin(); it < supported_emulators.end(); it++) {
        fs::path db_path = emulators_folder / it->get()->getName() / "db.dat"; //constructing the path in which we will find the database
        if( !(temp_res = it->get()->loadDB(db_path)) ) { temp_res.setDescription(it->get()->getName()); return temp_res; } //if one of the loading goes wrong we return the errorr
    }

    return Result(Result::SUCCESS);
}

GeneralEmulator::emulator_pair GeneralEmulator::chooseEmulator(const std::vector<fs::path>& theRomsPath, const fs::path& theBasePath) const {
    std::string rom_name;
    if( !theRomsPath.empty() ) rom_name = theRomsPath[0].stem().string(); //getting the actual rom name
    std::vector<std::shared_ptr<Emulator>> emulator_choices;

    //choosing only the emulators which support this rom
    for( auto it = supported_emulators.begin(); it < supported_emulators.end(); it++ )
        if( it->get()->isRomSupported(rom_name) )
            emulator_choices.push_back(*it);

    //scrolling among all the emulators which support this rom to choose the best one
    std::vector<emulator_pair> best_pairs;
    for(auto it_path = theRomsPath.begin(); it_path < theRomsPath.end(); it_path++)
            for(auto it_emu = emulator_choices.begin(); it_emu < emulator_choices.end(); it_emu++)
                if( it_emu->get()->validateRom(*it_path, theBasePath) )
                    best_pairs.push_back(std::make_pair(*it_emu, *it_path));

    //this should be the algorithm which chooses the best emulator among all the suitable one, as of now it just makes the most conservative choice
    if( !best_pairs.empty() ) {
        std::vector<int> best_romsets;
        for(auto it = best_pairs.begin(); it < best_pairs.end(); it++) best_romsets.push_back(std::stoi(it->first.get()->getRomset().substr(it->first.get()->getRomset().find(".")+1)));
        unsigned int min_pos = std::distance(best_romsets.begin(), std::min_element(best_romsets.begin(), best_romsets.end()));
        return best_pairs[min_pos];
    }

    else return std::make_pair(std::shared_ptr<Emulator>(), fs::path()); //if no one is suited we return an empty pair
}

Result GeneralEmulator::checkRomLaunchability(const std::vector<fs::path>& theRomsPath, const fs::path& theBasePath) const {
    std::string rom_name;
    if( !theRomsPath.empty() ) rom_name = theRomsPath[0].stem().string(); //getting the actual rom name
    else return(ERR_INVALID_VECTOR);

    if( !isRomSupported(rom_name) ) return Result(ERR_ROM_NOT_SUPPORTED); //if the rom is not supported why even bother?

    if( !isRomRunnable(rom_name) ) return Result(ERR_ROM_NOT_RUNNABLE); //if the rom is not a runnable one why even bother?

    emulator_pair best_pair = chooseEmulator(theRomsPath, theBasePath); //we choose the best emulator to run the rom

    if( best_pair.first == nullptr ) return ERR_ROM_NOT_VALID; //if we didn't find any suitable emulator to launch it

    if( !fs::exists(best_pair.second) ) return Result(ERR_ROM_NOT_ACCESSIBLE); //if we can't find the rom on the filesystem

    return Result(Result::SUCCESS);
}

Result GeneralEmulator::runRom(const std::vector<fs::path>& theRomsPath, const fs::path& theBasePath) const {
    std::string rom_name;
    if( !theRomsPath.empty() ) rom_name = theRomsPath[0].stem().string(); //getting the actual rom name
    else return(ERR_INVALID_VECTOR);

    if( !isRomSupported(rom_name) ) return Result(ERR_ROM_NOT_SUPPORTED); //if the rom is not supported why even bother?

    if( !isRomRunnable(rom_name) ) return Result(ERR_ROM_NOT_RUNNABLE); //if the rom is not a runnable one why even bother?

    emulator_pair best_pair = chooseEmulator(theRomsPath, theBasePath); //we choose the best emulator to run the rom

    if( best_pair.first == nullptr ) return ERR_ROM_NOT_VALID; //if we didn't find any suitable emulator to launch it

    if( !fs::exists(best_pair.second) ) return Result(ERR_ROM_NOT_ACCESSIBLE); //if we can't find the rom on the filesystem

    writeToLog("Preparing to launch the rom with "+best_pair.first.get()->getName()+"...");

    if( !best_pair.first.get()->runRom(best_pair.second, theBasePath) ) return Result(ERR_EMULATOR);

    return Result(Result::SUCCESS);
}
