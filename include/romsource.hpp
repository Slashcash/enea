#ifndef ROMSOURCE_HPP
#define ROMSOURCE_HPP

#include <filesystem>
#include <list>
#include <optional>
#include <system_error>
#include <variant>
#include <vector>

class Rom;

class RomSource
{
 private:
    std::filesystem::path mPath;

    [[nodiscard]] virtual std::optional<std::error_code> folderExists(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::list<std::filesystem::path> scanFolder(const std::filesystem::path& path) const;

 public:
    enum class Error
    {
        DIRECTORY_NOT_EXISTING
    };

    RomSource() = delete;
    explicit RomSource(const std::filesystem::path& path);

    [[nodiscard]] std::filesystem::path path() const;

    [[nodiscard]] std::variant<std::vector<Rom>, Error> scan() const;

    virtual ~RomSource() = default;
};

#endif // ROMSOURCE_HPP