#ifndef ROMSOURCE_HPP
#define ROMSOURCE_HPP

#include <filesystem>
#include <list>
#include <optional>
#include <stdexcept>
#include <system_error>

#include <rocket.hpp>

class Rom;

class RomSource
{
 private:
    std::filesystem::path mPath;

    [[nodiscard]] virtual std::optional<std::error_code> folderExists(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::list<std::filesystem::path> scanFolder(const std::filesystem::path& path) const;
    inline virtual void addRom(const Rom& rom) const
    {
        romAdded(rom);
    }

 public:
    enum class Error
    {
        INVALID_PATH
    };

    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    RomSource() = delete;
    explicit RomSource(const std::filesystem::path& path);

    [[nodiscard]] inline std::filesystem::path path() const
    {
        return mPath;
    };

    [[nodiscard]] std::optional<Error> monitor() const;

    virtual ~RomSource() = default;

    rocket::signal<void(const Rom& rom)> romAdded;
};

#endif // ROMSOURCE_HPP