#ifndef ROMSOURCE_HPP
#define ROMSOURCE_HPP

#include <filesystem>
#include <list>
#include <optional>

#include "exception.hpp"

class RomSource
{
 public:
    struct ScanResult
    {
        std::string lastModified;
        std::list<std::filesystem::path> roms;
        std::list<std::filesystem::path> media;
    };

 private:
    std::filesystem::path mRomFolder;

    [[nodiscard]] virtual bool isRomFolderValid() const;
    [[nodiscard]] virtual std::list<std::filesystem::path> scanFolder(const std::filesystem::path& path) const;
    [[nodiscard]] virtual std::optional<std::string> lastFolderModification(const std::filesystem::path& path) const;
    [[nodiscard]] static bool fileIsImage(const std::filesystem::path& path);
    [[nodiscard]] static bool fileIsRom(const std::filesystem::path& path);

 public:
    enum class Error
    {
        INVALID_ROM_PATH
    };

    class Excep : public Exception
    {
        using Exception::Exception;
    };

    RomSource() = delete;
    explicit RomSource(std::filesystem::path romFolder);
    RomSource(const RomSource& source) = delete;
    RomSource(RomSource&& source) = delete;

    [[nodiscard]] std::filesystem::path romFolder() const;
    [[nodiscard]] ScanResult scan() const;
    [[nodiscard]] std::string lastEditTime() const;

    RomSource& operator=(const RomSource& source) = delete;
    RomSource& operator=(RomSource&& source) = delete;
    bool operator==(const RomSource& source) = delete;

    virtual ~RomSource() = default;
};

#endif // ROMSOURCE_HPP
