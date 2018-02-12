#include "result.hpp"

Result::Result(const unsigned int theErrorNumber, const std::string& theDescription) {
    error_number = theErrorNumber;
    description = theDescription;
}
