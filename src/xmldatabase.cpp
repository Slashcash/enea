#include "xmldatabase.hpp"

#include <string>

XMLDataBase::XMLDataBase() {
    root_element = nullptr; //this is here for safety
}

const tinyxml2::XMLElement* XMLDataBase::findRom(const std::string& theName) const {
    const tinyxml2::XMLElement* temp = root_element; //transferint root_element to a temporary variable as we can't lose it
    const tinyxml2::XMLAttribute* temp_attribute;

    //scroll through the xml and stop when it finishes, or when we find the needed rom
    while( temp != nullptr && ((temp_attribute = temp->FindAttribute("name")) != nullptr) && temp_attribute->Value() != theName)
        temp = temp->NextSiblingElement("game");

    return temp;
}

const tinyxml2::XMLElement* XMLDataBase::getElement(const tinyxml2::XMLElement* const theRom, const std::string& theName) const {
    if( theRom == nullptr  ) return nullptr;
    return theRom->FirstChildElement(theName.c_str());
}

std::string XMLDataBase::getElementValue(const tinyxml2::XMLElement* const theRom, const std::string& theName) const {
    const tinyxml2::XMLElement* element_ptr;
    if( (element_ptr = getElement(theRom, theName)) == nullptr ) return "";

    return element_ptr->GetText();
}

const tinyxml2::XMLAttribute* XMLDataBase::getAttribute(const tinyxml2::XMLElement* const theElement, const std::string& theName) {
    if( theElement == nullptr ) return nullptr;
    else return theElement->FindAttribute(theName.c_str());
}

std::string XMLDataBase::getAttributeValue(const tinyxml2::XMLElement* const theElement, const std::string& theName) const {
    if( theElement == nullptr ) return nullptr;
    const tinyxml2::XMLAttribute* temp = theElement->FindAttribute(theName.c_str());
    if( temp == nullptr ) return "";
    else return temp->Value();
}

Result XMLDataBase::loadFromFileEffectively(const fs::path& thePath) {
    //it may seem redundant as the db is constructed with this same instruction but technically you may want to reload an already loaded db
    root_element = nullptr;

    if( xml_document.LoadFile(thePath.c_str()) != tinyxml2::XMLError::XML_SUCCESS ) return Result(LOAD_ERROR); //use tinyxml2 to load the document in memory


    const tinyxml2::XMLElement* temp_root_element = xml_document.RootElement(); //search for the root element (which should cointain some control metadata)
    if( temp_root_element == nullptr ) return Result(CORRUPT_FILE_ERROR);

    //if it has no child it means the db file is in some way corrupted
    if( ((root_element = temp_root_element->FirstChildElement()) == nullptr) ) return Result(INVALID_FILE_ERROR);

    return Result(Result::SUCCESS);
}

std::vector<RomPart> XMLDataBase::getRomParts(const std::string& theRomName) const {
    std::vector<RomPart> buffer;

    const tinyxml2::XMLElement* rom_element = getElement(findRom(theRomName), "rom");


    //cycle through all the rom parts which form this rom
    while( rom_element != nullptr && (rom_element->Name() == std::string("rom") || rom_element->Name() == std::string("disk")) ) {
        std::string name = getAttributeValue(rom_element, "name");
        std::string merge = getAttributeValue(rom_element, "merge");
        bool is_disk;

        std::string status_str = getAttributeValue(rom_element, "status");
        RomPart::Status status;
        if( status_str == "baddump" ) status = RomPart::Status::BAD_DUMP;
        else if( status_str == "nodump" ) status = RomPart::Status::NO_DUMP;
        else status = RomPart::Status::GOOD_DUMP;

        if( rom_element->Name() == std::string("disk") ) is_disk = true;
        else is_disk = false;

        std::string crc;
        if( status != RomPart::Status::NO_DUMP && !is_disk ) crc  = getAttributeValue(rom_element, "crc");
        else crc = "0";

        std::string sha1;
        if( status != RomPart::Status::NO_DUMP ) sha1  = getAttributeValue(rom_element, "sha1");
        else sha1 = "0";

        buffer.push_back(RomPart(name, std::stoul(crc, nullptr, 16), sha1, merge, status, is_disk)); //since we don't use sha1 anyway i put it as 0 here, be sure to change this if sha1 gets used somewhere
        rom_element = rom_element->NextSiblingElement();
    }

    return buffer;
}
