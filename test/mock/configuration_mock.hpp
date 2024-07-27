#ifndef CONFIGURATIONMOCK_HPP
#define CONFIGURATIONMOCK_HPP

#include "configuration.hpp"

#include <gmock/gmock.h>

class ConfigurationMock : public Conf
{
 public:
    using Conf::Conf;

    MOCK_METHOD(std::optional<std::filesystem::path>, homeDirectory, (), (const override));
};

#endif // CONFIGURATIONMOCK_HPP
