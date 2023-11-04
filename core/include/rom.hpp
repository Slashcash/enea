#ifndef ROM_HPP
#define ROM_HPP

#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>

#include "romdb.hpp"

class Rom
{
 private:
    std::filesystem::path mPath;
    RomDB::RomInfo mRomInfo;

    [[nodiscard]] virtual int runEmulator(const std::filesystem::path& romPath) const;
    [[nodiscard]] virtual std::optional<std::error_code> fileExists(const std::filesystem::path& path) const;
    void checkPathValidity() const;

    friend void from_json(const nlohmann::json& j, Rom& rom);
    friend void to_json(nlohmann::json& j, const Rom& rom);

 public:
    enum class Error
    {
        EMULATOR_ERROR,
        INVALID_ROM_FILE
    };

    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    Rom() = delete;
    explicit Rom(const std::filesystem::path& path, const RomDB::RomInfo& romInfo = RomDB::RomInfo());
    explicit Rom(const nlohmann::json& j);

    [[nodiscard]] inline bool operator==(const Rom& rom) const
    {
        return this->mPath == rom.mPath;
    }
    [[nodiscard]] inline std::filesystem::path path() const
    {
        return mPath;
    }
    [[nodiscard]] inline std::string fileName() const
    {
        return mPath.filename();
    }
    [[nodiscard]] inline std::string name() const
    {
        return mRomInfo.name.has_value() ? mRomInfo.name.value() : mPath.stem().string();
    }
    [[nodiscard]] inline std::optional<std::string> year() const
    {
        return mRomInfo.year;
    }
    [[nodiscard]] inline std::optional<std::string> manufacturer() const
    {
        return mRomInfo.manufacturer;
    }

    inline void setRomInfo(const RomDB::RomInfo& romInfo)
    {
        mRomInfo = romInfo;
    }

    [[nodiscard]] std::optional<Error> launch() const;
};

#endif // ROM_HPP