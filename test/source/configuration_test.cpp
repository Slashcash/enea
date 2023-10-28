#include "configuration_mock.hpp"

#include <gtest/gtest.h>

#include "version.hpp"

static const std::filesystem::path home = "/home";
static const std::filesystem::path tmp = "/tmp";

TEST(configuration, romDirectory)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.romDirectory(), home / ("." + static_cast<std::string>(executableName)) / "roms");
}

TEST(configuration, cacheDirectory)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.cacheDirectory(), home / ("." + static_cast<std::string>(executableName)) / "cache");
}

TEST(configuration, romDirectory_noHomeEnv)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(config, tempHomeDirectory()).WillOnce(testing::Return(tmp));

    EXPECT_EQ(config.romDirectory(), tmp / ("." + static_cast<std::string>(executableName)) / "roms");
}

TEST(configuration, romDirectory_noHome)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(config, tempHomeDirectory()).WillOnce(testing::Return(std::nullopt));

    EXPECT_THROW(config.romDirectory(), Configuration::Exception);
}