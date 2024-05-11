#ifndef SYSTEMCOMMANDMOCK_HPP
#define SYSTEMCOMMANDMOCK_HPP

#include <gmock/gmock.h>

#include "systemcommand.hpp"

class SystemCommandMock : public SystemCommand
{
 public:
    using SystemCommand::SystemCommand;

    MOCK_METHOD(SystemCommand::Result, launchCmd, (), (const override));
};

#endif // SYSTEMCOMMANDMOCK_HPP
