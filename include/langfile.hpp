#ifndef _LANGFILE_HPP_
#define _LANGFILE_HPP_

#include <map>

#include "result.hpp"
#include "filesystem.hpp"

class LangFile {
    private:
        static const std::string SEPARATOR;

        std::map<const std::string, std::string> lang_map;

    public:
        enum Error { //a lot of things can go wrong
            LOAD_ERROR = 1,
            PARSING_ERROR
        };

        LangFile() = default;

        Result load(const fs::path& thePath);
        std::string getValue(const std::string& theKey) const;
};

#endif //_LANGFILE_HPP_
