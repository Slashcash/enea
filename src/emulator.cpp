#include "emulator.hpp"

Emulator::Emulator(const fs::path& theEmulatorPath, const std::string& theEmulatorName, const std::string& theEmulatorRomset) {
    emulator_name = theEmulatorName;
    emulator_path = theEmulatorPath;
    emulator_romset = theEmulatorRomset;
}

Result Emulator::runRom(const fs::path& theRomPath) const {
    writeToLog("Running "+theRomPath.string()+"...");
    return runEffectively(theRomPath);
}

bool Emulator::validateRom(const fs::path& theRomPath) const {
    std::string rom_name = theRomPath.stem();

    std::vector<RomPart> parts = getRomParts(rom_name); //asking the database for all the different parts of a rom

    //opening the zip file of a rom
    mz_zip_archive original_zip;
    mz_zip_zero_struct(&original_zip);
    if( !mz_zip_reader_init_file(&original_zip, theRomPath.string().c_str(), 0) ) return false; //if we can't open it why even bother?

    //we check if the rom has a parent
    mz_zip_archive merged_zip;
    mz_zip_zero_struct(&merged_zip);
    bool rom_to_merge_found = false; //this variable is used to store if we have or have not found the parent rom
    std::string rom_to_merge = getRomOrigin(rom_name);

    if( !rom_to_merge.empty() ) { //opening the parent only if the rom effectively has one
        fs::path path_rom_to_merge = theRomPath;
        path_rom_to_merge.replace_filename(rom_to_merge+".zip");
        if( mz_zip_reader_init_file(&merged_zip, path_rom_to_merge.string().c_str(), 0) ) rom_to_merge_found = true;
    }

    for(unsigned int i = 0; i < parts.size(); i++) { //we check, for every part of the rom, if it exists in the rom or in its parent
        if( parts[i].getStatus() != RomPart::Status::NO_DUMP ) { //skip the check if a rom has not been correctly dumped
            if( !validatePartInArchive(original_zip, parts[i]) ) { //if this part is not present in the rom we try to find it in the parent
                if( parts[i].toBeMerged() && rom_to_merge_found ) { //if the parents exists...
                    if( !validatePartInArchive(merged_zip, parts[i]) ) return false; //we search in it...
                }

                else return false;
            }
        }
    }

    mz_zip_reader_end(&original_zip);
    if( rom_to_merge_found ) mz_zip_reader_end(&merged_zip);

    return true;
}

bool Emulator::validatePartInArchive(mz_zip_archive& theArchive, const RomPart& thePart) const {
    mz_uint32 index;
    if( !mz_zip_reader_locate_file_v2(&theArchive, thePart.getName().c_str(), nullptr, 0, &index) ) return false; //check if the file exists in the archive

    //we extract the file to heap to calculate its crc
    std::size_t buffer_size;
    unsigned char* file_ptr = (unsigned char*)mz_zip_reader_extract_to_heap(&theArchive, index, &buffer_size, 0);
    if( file_ptr == nullptr ) return false; //extraction failed

    //comparing crc's
    if( thePart.getCRC() != mz_crc32(0, file_ptr, buffer_size) ) return false;

    delete [] file_ptr;
    return true;
}
