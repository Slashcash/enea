#include "database.hpp"

Result DataBase::loadFromFile(const fs::path& theDBPath) {
    writeToLog("Loading database at "+theDBPath.string()+"...");
    Result temp_res = loadFromFileEffectively(theDBPath);

    if( !temp_res ) {
        temp_res.setDescription("Unable to load database");
        writeToLog(temp_res, LogWriter::Type::ERROR);
    }

    return temp_res;
}
