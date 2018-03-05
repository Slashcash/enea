#include "filesystem.hpp"

Result FileSystem::openStream(std::fstream& theStream, const path& thePath, const Mode& theMode) {
    //setting the correct open mode
    std::ios_base::openmode open_mode;

    if( theMode == INPUT ) open_mode = std::ios::in;
    else if( theMode == OUTPUT ) open_mode = std::ios::out;
    else if( theMode == INPUT_AND_OUTPUT ) open_mode = std::ios::in | std::ios::out;
    else return Result(ERR_UNKNOWN_OPEN_MODE); //just in case someone wants to be funny and passes an invalid mode

    theStream.open(thePath, open_mode); //effectively opening the stream towards the file

    //returning a result
    if( theStream.is_open() ) return Result(Result::SUCCESS);
    else return Result(ERR_OPEN_STREAM);
}

Result FileSystem::moveFile(const path& theOldLocation, const path& theNewLocation) {
    std::error_code os_error_code;
    writeToLog("Moving "+theOldLocation.string()+" to "+theNewLocation.string()+"...");

    rename(theOldLocation, theNewLocation, os_error_code);
    Result temp_res(os_error_code.value(), os_error_code.message()); //building an error message eventually
    if( !temp_res ) writeToLog(temp_res, LogWriter::Type::ERROR);
    return temp_res; //i am unreliably and stupidly converting an int to an unsigned int, if something goes wrong is my fault!
}

Result FileSystem::removeFile(const path& theFilePath) {
    std::error_code os_error_code;
    writeToLog("Removing "+theFilePath.string()+"...\n");

    remove(theFilePath, os_error_code);
    Result temp_res(os_error_code.value(), os_error_code.message()); //building an error message eventually
    if( !temp_res ) writeToLog(temp_res, LogWriter::Type::ERROR);
    return temp_res; //i am unreliably and stupidly converting an int to an unsigned int, if something goes wrong is my fault!
}

Result FileSystem::copyFile(const path& theOldLocation, const path& theNewLocation) {
    std::error_code os_error_code;
    writeToLog("Copying "+theOldLocation.string()+" to "+theNewLocation.string()+"...\n");

    copy_file(theOldLocation, theNewLocation, os_error_code);
    Result temp_res(os_error_code.value(), os_error_code.message()); //building an error message eventually
    if( !temp_res ) writeToLog(temp_res, LogWriter::Type::ERROR);
    return temp_res; //i am unreliably and stupidly converting an int to an unsigned int, if something goes wrong is my fault!
}
