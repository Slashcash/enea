#ifndef ROMSOURCEMOCK_HPP
#define ROMSOURCEMOCK_HPP

#include "romsource.hpp"

#include <gmock/gmock.h>

class RomSourceMock : public RomSource
{
 public:
    using RomSource::RomSource;

    MOCK_METHOD(std::optional<std::error_code>, folderExists, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::list<std::filesystem::path>, scanFolder, (const std::filesystem::path& path), (const override));
};

#endif // ROMSOURCEMOCK_HPP