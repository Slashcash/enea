#ifndef _LOGWRITER_HPP_
#define _LOGWRITER_HPP_

#include <iostream>
#include <experimental/filesystem>
#include <string>

#include <mutex>

#include "result.hpp"

/*Inherit from this class if you want to have a consistent log writing experience with your class*/

namespace fs = std::experimental::filesystem::v1; //we have a filesystem class but we don't use it as this class is really particular, we prefer to manage everything in class

class LogWriter {
    private:
        //members
        static std::mutex log_mutex; //this mutex protects the shared stream resources (as ideally this class has to be used even in different threads)
        static std::ostream& console_log; //a reference to the console log ( usually clog :D )
        static std::ofstream file_log; //if the log gets redirected to a file we store the file stream here
        static bool redirect_to_file; //this helps us knowing if the stream has been redirected to a file

    protected:
        //ctor
        LogWriter() = default;

        //static functions
        static Result redirectToFile(const fs::path& thePath); //use this if you want to redirect the stream to a file
        static void redirectToConsole() { redirect_to_file = false; } //redirects it back to the console
        static void writeToLog(const std::string& theMsg);

    public:
        //enum
        enum Error {
            ERR_OPEN_LOGFILE = 1
        };
};

#endif // _LOGWRITER_HPP_
