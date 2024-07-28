#ifndef ROMSOURCEMOCK_HPP
#define ROMSOURCEMOCK_HPP

#include "romsource.hpp"

#include <gmock/gmock.h>

class RomSourceMock : public RomSource
{
 public:
    using RomSource::RomSource;
    MOCK_METHOD(bool, isFolder, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::vector<std::filesystem::path>, scanFolder, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::optional<RomInfo>, romInfo, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::optional<std::string>, lastFolderModification, (const std::filesystem::path& path),
                (const override));
    MOCK_METHOD(bool, writeCacheFile, (const nlohmann::json& json, const std::filesystem::path& path),
                (const override));
    MOCK_METHOD(std::optional<nlohmann::json>, readCacheFile, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::string_view, version, (), (const override));
};

#endif // ROMSOURCEMOCK_HPP
