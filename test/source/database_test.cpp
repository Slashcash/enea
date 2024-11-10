#include "database_mock.hpp"

#include <gtest/gtest.h>

class DatabaseFixture : public ::testing::Test
{
 protected:
    static constexpr auto DB_PATH = "test.json";

    DatabaseMock db{DB_PATH};
};

TEST_F(DatabaseFixture, findDatabaseNoValues)
{
    /*
        Getting information from an invalid database (misses the "values" field)
        Expectation: the function should throw.
    */
    const nlohmann::json json{{"test", "invalid"}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));
    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

TEST_F(DatabaseFixture, findDatabaseNoArray)
{
    /*
        Getting information from an invalid database (the "values" field is not an array")
        Expectation: the function should throw.
    */
    const nlohmann::json json{{"values", "invalid"}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));
    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

TEST_F(DatabaseFixture, findDatabaseNoKey)
{
    /*
        Getting information from an invalid database (one of the element
        of the "values" field does not have a key")
        Expectation: the function should throw.
    */
    const nlohmann::json json{{"values", {{"invalid", "value"}}}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));
    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

TEST_F(DatabaseFixture, findDatabaseNoValue)
{
    /*
        Getting information from an invalid database (one of the element
        of the "values" field does not have a associated information")
        Expectation: the function should throw.
    */
    const nlohmann::json json{{"values", {{{"key", "id"}, {"invalid", "value"}}}}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));
    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

TEST_F(DatabaseFixture, findDatabaseLoadFailed)
{
    /*
        Getting information from an incorrectly loaded database
        Expectation: the function should throw first time due to load operation failure.
        Second time there should be no load operation called (as it would be useless)
        and we should directly return an empty value
    */
    const nlohmann::json json{{"values", {{{"key", "id"}, {"invalid", "value"}}}}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));
    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
    EXPECT_CALL(db, loadFromFile(DB_PATH)).Times(0);
    EXPECT_EQ(db.find("sf2"), std::nullopt);
}

TEST_F(DatabaseFixture, findAvailableValue)
{
    /*
        Getting available information from a correctly loaded database
        Expectation: the function should return the needed info.
    */
    const std::string KEY = "sf2";
    const std::string VALUE = "Street Fighter II";

    const nlohmann::json json{{"values", {{{"key", KEY}, {"info", VALUE}}}}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));

    auto info = db.find(KEY);
    EXPECT_TRUE(info && *info == VALUE);
}

TEST_F(DatabaseFixture, findValueAlreadyLoadedDatabase)
{
    /*
        Getting available information from a correctly loaded database two times.
        Expectation: the second time there should be no need to re-load the database.
        And information should be correctly retrieved
    */
    const std::string KEY = "sf2";
    const std::string VALUE = "Street Fighter II";

    const nlohmann::json json{{"values", {{{"key", KEY}, {"info", VALUE}}}}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));
    auto info = db.find(KEY);
    EXPECT_TRUE(info && *info == VALUE);

    EXPECT_CALL(db, loadFromFile(DB_PATH)).Times(0);
    info = db.find(KEY);
    EXPECT_TRUE(info && *info == VALUE);
}

TEST_F(DatabaseFixture, findUnavailableValue)
{
    /*
        Getting available information from a correctly loaded database
        Expectation: the function should return the needed info.
    */
    const nlohmann::json json{{"values", {{{"key", "mslug"}, {"info", "Metal Slug"}}}}};
    EXPECT_CALL(db, loadFromFile(DB_PATH)).WillOnce(testing::Return(json));
    EXPECT_FALSE(db.find("sf2"));
}
