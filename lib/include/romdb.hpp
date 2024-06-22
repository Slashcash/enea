#ifndef ROMDB_HPP
#define ROMDB_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "exception.hpp"
#include "rominfo.hpp"

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

#endif // ROMDB_HPP
