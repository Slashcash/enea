#ifndef ROM_HPP
#define ROM_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <system_error>

class Rom
{
 private:
    std::filesystem::path mPath;

    [[nodiscard]] virtual std::optional<std::error_code> fileExists(const std::filesystem::path& path) const;
    [[nodiscard]] virtual int runEmulator(const std::filesystem::path& romPath) const;

 public:
    enum class Error
    {
        PATH_NOT_ABSOLUTE,
        ROM_FILE_NOT_EXISTING,
        EMULATOR_ERROR
    };

    Rom() = delete;
    explicit Rom(const std::filesystem::path& path);

    [[nodiscard]] std::filesystem::path path() const;
    [[nodiscard]] std::string fileName() const;
    [[nodiscard]] std::string name() const;

    [[nodiscard]] std::optional<Error> launch() const;

    virtual ~Rom() = default;
};

#endif // ROM_HPP