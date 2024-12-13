#ifndef ROMFOLDER_HPP
#define ROMFOLDER_HPP

#include "rom/source.hpp"

namespace Rom {

class Folder : public Source
{
 private:
    std::filesystem::path mFolderPath;

 public:
    explicit inline Folder(const std::filesystem::path& folderPath, const std::filesystem::path& folderCache)
        : Source(folderPath.string(), folderCache), mFolderPath(folderPath)
    {}

 private:
    [[nodiscard]] std::vector<std::filesystem::path> scan() const override;
    [[nodiscard]] std::vector<std::filesystem::path> fileList(const std::filesystem::path& folder) const;
    [[nodiscard]] std::optional<std::string> lastModified() const override;
};
} // namespace Rom

#endif // ROMFOLDER_HPP
