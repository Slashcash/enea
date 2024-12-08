#ifndef ROMSOURCEMOCK_HPP
#define ROMSOURCEMOCK_HPP

#include "rom/source.hpp"

#include <gmock/gmock.h>

namespace Rom {
class SourceMock : public Source
{
 public:
    using Source::Source;
    MOCK_METHOD(std::vector<std::filesystem::path>, scan, (), (const override));
    MOCK_METHOD(std::optional<Rom::Info>, romInfo, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::optional<std::string>, lastModified, (), (const override));
    MOCK_METHOD(bool, writeCacheFile, (const nlohmann::json& json, const std::filesystem::path& path),
                (const override));
    MOCK_METHOD(std::optional<nlohmann::json>, readCacheFile, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::string_view, version, (), (const override));
};
} // namespace Rom
#endif // ROMSOURCEMOCK_HPP
