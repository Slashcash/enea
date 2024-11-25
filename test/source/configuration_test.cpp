#include "configuration_mock.hpp"

#include <gtest/gtest.h>

#include "softwareinfo.hpp"

static const std::filesystem::path home = "/home";
static const std::filesystem::path base = home / ("." + std::string(executableName));

/*
    Asking for the folder where to store the roms.
    Expectation: we get an hidden folder constructed with the home directory + .enea + roms.
*/
TEST(Configuration, romDirectory)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.romDirectory(), base / "roms");
}

/*
    Asking for the folder where to store the roms but we fail to retrieve home.
    Expectation: we throw.
*/
TEST(Configuration, romDirectoryNoHome)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));

    EXPECT_THROW(config.romDirectory(), ConfigurationMock::Exception);
}

/*
    Asking for the directory where to store the rom cache.
    Expectation: we get a directory name constructed with the home directory + .enea + cache.
*/
TEST(Configuration, cacheDirectory)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.cacheDirectory(), base / "cache");
}

/*
    Asking for directory where to store the rom cache but we fail to retrieve home.
    Expectation: we throw.
*/
TEST(Configuration, cacheDirectoryNoHome)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));

    EXPECT_THROW(config.cacheDirectory(), ConfigurationMock::Exception);
}

/*
    Asking for the advMame configuration file.
    Expectation: we get a file name constructed with home + .enea + advmame.rc
*/
TEST(Configuration, advMameConfigurationFile)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.advMameConfigurationFile(), base / "advmame.rc");
}

/*
    Asking for the advMame configuration file. but we fail to retrieve home
    Expectation: we throw.
*/
TEST(Configuration, advMameConfigurationFileNoHome)
{
    ConfigurationMock config;
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));

    EXPECT_THROW(config.advMameConfigurationFile(), ConfigurationMock::Exception);
}
