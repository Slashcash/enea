#ifndef ROM_HPP
#define ROM_HPP

#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>

class Rom
{
 private:
    std::filesystem::path mPath;
    std::optional<std::string> mName;

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
    explicit Rom(const std::filesystem::path& path);
    explicit Rom(const nlohmann::json& j);

    [[nodiscard]] inline std::filesystem::path path() const noexcept
    {
        return mPath;
    }
    [[nodiscard]] inline std::string fileName() const noexcept
    {
        return mPath.filename();
    }
    [[nodiscard]] inline std::string name() const noexcept
    {
        return mName.has_value() ? mName.value() : mPath.stem().string();
    }

    [[nodiscard]] std::optional<Error> launch() const;
};

#endif // ROM_HPP