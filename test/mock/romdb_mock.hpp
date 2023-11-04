#ifndef ROMDBMOCK_HPP
#define ROMDBMOCK_HPP

#include "romdb.hpp"

#include <gmock/gmock.h>

class RomDBMock : public RomDB
{
 public:
    using RomDB::RomDB;
    MOCK_METHOD((std::pair<std::optional<Error>, std::string>), loadFromFile, (const std::filesystem::path& path),
                (const override));
};

#endif // ROMDBMOCK_HPP