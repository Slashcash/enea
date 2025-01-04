#include "systemcommand_mock.hpp"

#include <gtest/gtest.h>

/*
    Launching a command with a failure
    Expectation: the error code is returned
*/
TEST(SystemCommand, launchFailure)
{
    SystemCommandMock cmd{"./advmame"};

    EXPECT_CALL(cmd, launchCmd())
        .WillOnce(testing::Return(ChefFun::Either<SYSTEM2_RESULT, SystemCommand::Output>::Left(
            SYSTEM2_RESULT::SYSTEM2_RESULT_PIPE_CREATE_FAILED)));

    auto result = cmd.launch();
    ASSERT_TRUE(result.isLeft());
    EXPECT_EQ(result.getLeft(), SystemCommand::Error::LAUNCH_COMMAND);
}

/*
    Succesfully launching a command
    Expectation: the process return code and output is returned
*/
TEST(SystemCommand, launch)
{
    SystemCommandMock cmd{"./advmame"};
    const int returnCode = 0;
    const std::string cmdOutput = "output";

    EXPECT_CALL(cmd, launchCmd())
        .WillOnce(testing::Return(ChefFun::Either<SYSTEM2_RESULT, SystemCommand::Output>::Right(
            SystemCommand::Output{returnCode, cmdOutput})));

    auto result = cmd.launch();
    ASSERT_TRUE(result.isRight());
    EXPECT_EQ(result.getRight().exitCode, returnCode);
    EXPECT_EQ(result.getRight().output, cmdOutput);
}
