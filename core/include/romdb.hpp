#ifndef ROMDB_HPP
#define ROMDB_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <utility>

#include <tinyxml2.h>

class RomDB
{
 public:
    struct RomInfo
    {
        std::optional<std::string> name;
        std::optional<std::string> year;
        std::optional<std::string> manufacturer;
    };

    enum class Error
    {
        UNABLE_TO_LOAD_DB_FILE,
        UNABLE_TO_PARSE_DB_FILE,
        DB_FILE_NOT_LOADED
    };

 private:
    tinyxml2::XMLDocument mXML;
    tinyxml2::XMLElement* mRootElement = nullptr;
    std::filesystem::path mPath;
    bool mLoaded = false;

    [[nodiscard]] virtual std::pair<std::optional<Error>, std::string>
    loadFromFile(const std::filesystem::path& path) const;
    [[nodiscard]] tinyxml2::XMLElement* findRom(const std::string& rom) const;

 public:
    RomDB() = delete;
    inline explicit RomDB(const std::filesystem::path& path) : mPath(path){};

    [[nodiscard]] std::optional<Error> load();
    [[nodiscard]] std::optional<RomInfo> find(const std::string& rom) const;
};

#endif // ROMDB_HPP