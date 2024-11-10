#include "configuration_mock.hpp"

#include <gtest/gtest.h>

#include "softwareinfo.hpp"

static const std::filesystem::path home = "/home";
static const std::filesystem::path base = home / ("." + std::string(executableName));
static const std::filesystem::path roms = base / "roms";
static const std::filesystem::path cache = base / "cache";

class ConfigurationFixture : public ::testing::Test
{
 protected:
    ConfigurationMock config;
};

TEST_F(ConfigurationFixture, romDirectory)
{
    /*
        Asking for the folder where to store the roms.
        Expectation: We get an hidden folder constructed with the home directory + enea.
    */
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.romDirectory(), roms);
}

TEST_F(ConfigurationFixture, cacheFile)
{
    /*
        Asking for the file where to store the rom cache.
        Expectation: We get a file name constructed with the home directory + .enea + cache.json.
    */
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.cacheDirectory(), cache);
}

TEST_F(ConfigurationFixture, romDirectory_noHome)
{
    /*
        Asking for the place where to store the roms but we are unable to get the home directory.
        Expectation: We throw an exception.
    */
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(config.romDirectory(), Conf::Exception);
}

TEST_F(ConfigurationFixture, cacheFile_noHome)
{
    /*
        Asking for the file where to store the rom cache but we are unable to get the home directory.
        Expectation: We throw an exception.
    */
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(config.cacheDirectory(), Conf::Exception);
}
