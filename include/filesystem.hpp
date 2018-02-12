#ifndef _FILESYSTEM_HPP_
#define _FILESYSTEM_HPP_

#include <fstream>
#include <experimental/filesystem>

#include "result.hpp"
#include "logwriter.hpp"

/*A class that handles (not so) low level filesystem writing, it makes good use of c++11fs*/

namespace fs = std::experimental::filesystem::v1;
using namespace fs;

class FileSystem : virtual public LogWriter {
    public:
        //enum
        enum Error { //All the errors for this class
            ERR_UNKNOWN_OPEN_MODE = 1,
            ERR_OPEN_STREAM
        };

        enum Mode { //A stream can be opened in input or output
            INPUT,
            OUTPUT,
            INPUT_AND_OUTPUT
        };

        static Result openStream(std::fstream& theStream, const path& thePath, const Mode& theMode = INPUT_AND_OUTPUT); //opens a stream to a specified path, mode is INPUT or OUTPUT
        static Result moveFile(const path& theOldLocation, const path& theNewLocation);
        static Result removeFile(const path& theFilePath);
        static Result copyFile(const path& theOldLocation, const path& theNewLocation);
};

#endif // _FILESYSTEM_HPP_

