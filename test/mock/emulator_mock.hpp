#ifndef EMULATORMOCK_HPP
#define EMULATORMOCK_HPP

#include <gmock/gmock.h>

#include "emulator.hpp"

class EmulatorMock : public Emulator
{
 public:
    using Emulator::Emulator;

    MOCK_METHOD((SystemCommand::Result), launch, (const std::string& arguments), (const override));
    MOCK_METHOD(bool, romExists, (const Rom& rom), (const override));
    MOCK_METHOD(bool, romIsReadable, (const Rom& rom), (const override));
};

#endif // EMULATORMOCK_HPP
