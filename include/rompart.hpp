#ifndef _ROMPART_HPP_
#define _ROMPART_HPP_

#include <string>

/*This class abstracts the fact that a game's rom is generally formed by different roms each and every one with an unique
name and crc, it is used to check if a game's rom contains every needed rom*/

class RomPart {
    public:
        //enum
        enum Status { //status of a rom
            NO_DUMP,
            BAD_DUMP,
            GOOD_DUMP
        };

    private:
        //members
        std::string name; //the name of the part
        unsigned int crc; //the crc of the part
        std::string sha1; //the sha1 of the part
        std::string merge; //if this string is not empty this rom may be contained in another game's rom
        Status status; //this variable represents the rom status (ie: a NO_DUMP means that this rom has not been dumped yet so we don't have to search for it)
        bool is_disk; //if this variable is set to true this part has to be considered as an external disk

    public:
        //ctor
        RomPart() = default;
        RomPart(const std::string& theName, const unsigned int theCRC, const std::string& theSHA1, const std::string& theMerge, const Status theStatus, const bool isDisk);

        //getters
        std::string getName() const { return name; }
        unsigned int getCRC() const { return crc; }
        std::string getSHA1() const { return sha1; }
        std::string getMerge() const { return merge; }
        Status getStatus() const { return status; }
        bool toBeMerged() const { return !merge.empty(); }
        bool isDisk() const { return is_disk; }

        //setters
        void setName(const std::string& theName) { name = theName; }
        void setCRC(const unsigned int theCRC) { crc = theCRC; }
        void setSHA1(const unsigned long long int theSHA1) { sha1 = theSHA1; }
        void setMerge(const std::string& theMerge) { merge = theMerge; }
        void setStatus(const Status theStatus) { status = theStatus; }
        void setDisk(const bool theValue) { is_disk = theValue; }
};

#endif // _ROMPART_HPP_

