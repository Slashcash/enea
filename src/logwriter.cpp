#include "logwriter.hpp"

#include <fstream>

std::ostream& LogWriter::console_log = std::clog;
std::ofstream LogWriter::file_log;
bool LogWriter::redirect_to_file = false;
std::mutex LogWriter::log_mutex;

Result LogWriter::redirectToFile(const fs::path& thePath) {
    file_log.open(thePath.string().c_str(), std::ios::out);

    if( !file_log.is_open() ) { redirect_to_file = false; return Result(ERR_OPEN_LOGFILE); }
    else { redirect_to_file = true; return Result(Result::SUCCESS); }
}

void LogWriter::writeToLog(const std::string& theMsg) {
    log_mutex.lock(); //remember that console_log and file_log are shared resources and we want this class to be thread safe when writing to a log
    if( !redirect_to_file ) console_log << theMsg; //write it on console
    else file_log << theMsg; //or on a file
    log_mutex.unlock();
}
