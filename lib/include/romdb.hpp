#ifndef ROMDB_HPP
#define ROMDB_HPP

#include <filesystem>
#include <optional>
#include <string>

#include <tinyxml2.h>

#include "rominfo.hpp"

/**
 * @brief This class manages the database that contains information about roms.
 * The rom database is an xml file built using the 'advmame --listxml' command.
 * This xml file is then embedded into our executable binary.
 *
 */
class RomDB
{
 public:
    enum class Error
    {
        UNABLE_TO_PARSE_DB_FILE,
    };

 private:
    tinyxml2::XMLDocument mXML;
    tinyxml2::XMLElement* mRootElement = nullptr;
    bool mLoaded{false};

    [[nodiscard]] virtual std::string loadFromFile() const;
    [[nodiscard]] tinyxml2::XMLElement* findRom(const std::string& rom) const;

 public:
    RomDB() = default;
    RomDB(const RomDB& romdb) = delete;
    RomDB(RomDB&& romdb) = delete;

    [[nodiscard]] bool loaded() const;
    [[nodiscard]] std::optional<Error> load();
    [[nodiscard]] std::optional<RomInfo> find(const std::filesystem::path& rom) const;

    RomDB& operator=(const RomDB& romdb) = delete;
    RomDB& operator=(RomDB&& romdb) = delete;
    bool operator==(const RomDB& romdb) = delete;

    virtual ~RomDB() = default;
};

#endif // ROMDB_HPP
