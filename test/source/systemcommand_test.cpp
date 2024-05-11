#include "systemcommand_mock.hpp"

#include <gtest/gtest.h>

static const std::string CMD_OUTPUT = "lol";

class SystemCommandFixture : public ::testing::Test
{
 protected:
    SystemCommandMock cmd{"./advmame"};
};

TEST_F(SystemCommandFixture, launch)
{
    constexpr int SUCCESS = 0;

    EXPECT_CALL(cmd, launchCmd()).WillOnce(testing::Return(SystemCommand::Result{SUCCESS, CMD_OUTPUT}));
    auto result = cmd.launch();
    EXPECT_EQ(result.exitcode, SUCCESS);
    EXPECT_EQ(result.output, CMD_OUTPUT);
}

TEST_F(SystemCommandFixture, launchFailure)
{
    constexpr int FAILURE = 1;

    EXPECT_CALL(cmd, launchCmd()).WillOnce(testing::Return(SystemCommand::Result{FAILURE, CMD_OUTPUT}));
    auto result = cmd.launch();
    EXPECT_EQ(result.exitcode, FAILURE);
    EXPECT_TRUE(result.output.empty());
}
