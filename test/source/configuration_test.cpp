#include "configuration_mock.hpp"

#include <gtest/gtest.h>

#include "softwareinfo.hpp"

static const std::filesystem::path home = "/home";
static const std::filesystem::path base = home / ("." + std::string(executableName));
static const std::filesystem::path roms = base / "roms";

class ConfigurationFixture : public ::testing::Test
{
 protected:
    ConfigurationMock config;
};

TEST_F(ConfigurationFixture, romDirectory)
{
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.romDirectory(), roms);
}

TEST_F(ConfigurationFixture, cacheFile)
{
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(home));

    EXPECT_EQ(config.cacheFile(), base / "cache.json");
}

TEST_F(ConfigurationFixture, romDirectory_noHome)
{
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(config.romDirectory(), Configuration::Excep);
}

TEST_F(ConfigurationFixture, cacheFile_noHome)
{
    EXPECT_CALL(config, homeDirectory()).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(config.cacheFile(), Configuration::Excep);
}
