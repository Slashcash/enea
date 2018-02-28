#include "result.hpp"

Result::Result(const unsigned int theErrorNumber, const std::string& theDescription) {
    error_number = theErrorNumber;
    description = theDescription;
}

Result::operator std::string() const {
    if( description.empty() ) return std::to_string(error_number);
    else return std::to_string(error_number) + " " + description;
}
