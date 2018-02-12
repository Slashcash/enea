#include "langfile.hpp"

#include <fstream>

const std::string LangFile::SEPARATOR = "=";

Result LangFile::load(const fs::path& thePath) {
    lang_map.clear();
    std::ifstream stream;
    stream.open(thePath);

    if( !stream.is_open() ) return Result(LOAD_ERROR);


    std::string temp_buffer;
    while( std::getline(stream, temp_buffer) ) {
        std::size_t separator_position = temp_buffer.find(SEPARATOR);
        if( separator_position == std::string::npos ) return Result(PARSING_ERROR); //try to find the separator in the line

        std::string key = temp_buffer.substr(0, separator_position); //separate the key and the value using the separator position
        std::string value = temp_buffer.substr(separator_position+1, std::string::npos);
        lang_map.insert(std::make_pair(key, value));
    }


    return Result(Result::SUCCESS);
}

std::string LangFile::getValue(const std::string& theKey) const {
    auto it = lang_map.find(theKey);
    if( it == lang_map.end() ) return "";
    else return it->second;
}
