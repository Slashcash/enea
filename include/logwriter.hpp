#ifndef _LOGWRITER_HPP_
#define _LOGWRITER_HPP_

#include <iostream>
#include <experimental/filesystem>
#include <string>

#include <mutex>

#include "result.hpp"

/*Inherit (virtually?) from this class if you want to have a consistent log writing experience with your class*/

namespace fs = std::experimental::filesystem::v1; //we have a filesystem class but we don't use it as this class is really particular, we prefer to manage everything in class

class LogWriter {
    public:
        //enum
        enum Error {
            ERR_OPEN_LOGFILE = 1
        };

        //you can write different things on the log, and you can exclude or include log messages of certain types
        enum Type {
            MSG,
            WARNING,
            ERROR
        };

    private:
        //members
        static std::mutex log_mutex; //this mutex protects the shared stream resources (as ideally this class has to be used even in different threads)
        static std::ostream& console_log; //a reference to the console log ( usually clog :D )
        static std::ofstream file_log; //if the log gets redirected to a file we store the file stream here
        static bool redirect_to_file; //this helps us knowing if the stream has been redirected to a file
        static Type filter_type; //set this variable to something different to set the message filtering by type

    protected:
        //ctor
        LogWriter() = default;

        //static functions
        static Result redirectToFile(const fs::path& thePath); //use this if you want to redirect the stream to a file
        static void redirectToConsole() { redirect_to_file = false; } //redirects it back to the console
        static void writeToLog(const std::string& theMsg, const Type& theMsgType = MSG);
        static void filterType(const Type& theType) { filter_type = theType; } //use this function to filter out the log, MSG will show all messages, WARNING will show warnings and errors, ERRORS will show only errors
};

#endif // _LOGWRITER_HPP_
