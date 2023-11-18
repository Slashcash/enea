#ifndef ROMMOCK_HPP
#define ROMMOCK_HPP

#include "rom.hpp"

#include <gmock/gmock.h>

class RomMock : public Rom
{
 public:
    using Rom::Rom;
    MOCK_METHOD(std::optional<std::error_code>, fileExists, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(int, runEmulator, (), (const override));
};

#endif // ROMMOCK_HPP