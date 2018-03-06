#include "logwriter.hpp"

#include <fstream>

std::ostream& LogWriter::console_log = std::clog;
std::ofstream LogWriter::file_log;
bool LogWriter::redirect_to_file = false;
std::mutex LogWriter::log_mutex;
LogWriter::Type LogWriter::filter_type = LogWriter::Type::MSG;
bool LogWriter::log_enabled = true;

Result LogWriter::redirectToFile(const fs::path& thePath) {
    file_log.open(thePath.string().c_str(), std::ios::out);

    if( !file_log.is_open() ) { log_enabled = false; redirect_to_file = false; return Result(ERR_OPEN_LOGFILE); }
    else { log_enabled = true; redirect_to_file = true; return Result(Result::SUCCESS); }
}

void LogWriter::writeToLog(const std::string& theMsg, const Type& theMsgType) {
    if( log_enabled ) {
        //see if the message has to be filtered out or not
        bool to_show = false;
        std::string to_write = theMsg + "\n"; //appending a newline character

        if( theMsgType == WARNING ) to_write = "WARNING: " + to_write;
        else if( theMsgType == ERROR ) to_write = "ERROR: " + to_write;

        if( filter_type == MSG ) to_show = true;
        else if( filter_type == WARNING && (theMsgType == WARNING || theMsgType == ERROR) ) to_show = true;
        else if( filter_type == ERROR && theMsgType == ERROR ) to_show = true;

        //only writing if it is not filtered out
        if( to_show ) {
            log_mutex.lock(); //remember that console_log and file_log are shared resources and we want this class to be thread safe when writing to a log
            if( !redirect_to_file ) console_log << to_write; //write it on console
            else file_log << to_write; //or on a file
            log_mutex.unlock();
        }
    }
}
