#ifndef ROMDB_HPP
#define ROMDB_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "exception.hpp"
#include "rominfo.hpp"

/**
 * @brief This class represents a rom database. We retrieve information about roms from this.
 * This database is stored as a json file into our codebase, see: db/romdb.json,
 * This json is then embedded directly into our executable and read from there to fill the database.
 * This class is then wrapped into a singleton
 */
class RomDB
{
 public:
    class Excep : public Exception
    {
        using Exception::Exception;
    };

 private:
    std::unordered_map<std::string, RomInfo> mRecords;
    bool mLoaded{false};

    [[nodiscard]] virtual nlohmann::json loadFromFile() const;

 public:
    RomDB() = default;
    RomDB(const RomDB& romdb) = delete;
    RomDB(RomDB&& romdb) = delete;

    [[nodiscard]] bool loaded() const;
    void load();
    [[nodiscard]] std::optional<RomInfo> find(const std::filesystem::path& rom) const;

    RomDB& operator=(const RomDB& romdb) = delete;
    RomDB& operator=(RomDB&& romdb) = delete;
    bool operator==(const RomDB& romdb) = delete;

    virtual ~RomDB() = default;
};

class RomDatabase
{
 public:
    RomDatabase() = delete;

    [[nodiscard]] inline static RomDB& get()
    {
        static RomDB romdb;

        if (!romdb.loaded())
        {
            spdlog::debug("Loading rom information database");
            romdb.load();
        }

        return romdb;
    }
};

#endif // ROMDB_HPP
