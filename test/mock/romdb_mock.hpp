#ifndef ROMDBMOCK_HPP
#define ROMDBMOCK_HPP

#include "romdb.hpp"

#include <gmock/gmock.h>

class RomDBMock : public RomDB
{
 public:
    using RomDB::RomDB;
    MOCK_METHOD(nlohmann::json, loadFromFile, (), (const override));
};

#endif // ROMDBMOCK_HPP
