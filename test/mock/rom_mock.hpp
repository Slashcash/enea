#ifndef ROMMOCK_HPP
#define ROMMOCK_HPP

#include "rom.hpp"

#include <gmock/gmock.h>

class RomMock : public Rom
{
 public:
    using Rom::Rom;

    MOCK_METHOD(std::optional<RomInfo>, loadInfoFromDb, (const std::string&), (const override));
};

#endif // CONFIGURATIONMOCK_HPP
