#include "rompart.hpp"

RomPart::RomPart(const std::string& theName, const unsigned int theCRC, const std::string& theSHA1, const std::string& theMerge, const Status theStatus, const bool isDisk) {
    name = theName;
    crc = theCRC;
    merge = theMerge;
    status = theStatus;
    is_disk = isDisk;
    sha1 = theSHA1;
}
