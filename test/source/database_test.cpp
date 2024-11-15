#include "database_mock.hpp"

#include <gtest/gtest.h>

/*
    Trying to query a database that was not correctly loaded
    from file.

    Expectation: the query returns an empty result
 */
TEST(Database, fromInvalidFile)
{
    DatabaseMock db("test.json");
    EXPECT_CALL(db, load()).WillOnce(testing::Return(std::nullopt));
    EXPECT_FALSE(db.find("sf2"));

    // Re-querying again on an unloaded database should return an empty result
    // without an useless load retry
    EXPECT_CALL(db, load()).Times(0);
    EXPECT_FALSE(db.find("sf2"));
}

/*
    Trying to query an invalid database (the "values" field is missing)

    Expectation: the query returns an empty result
 */
TEST(Database, fromInvalidJsonMissingValuesArray)
{
    DatabaseMock db("test.json");
    const nlohmann::json json{{"invalid", "value"}};
    EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
    EXPECT_FALSE(db.find("sf2"));

    // Re-querying again on an unloaded database should return an empty result
    // without an useless load retry
    EXPECT_CALL(db, load()).Times(0);
    EXPECT_FALSE(db.find("sf2"));
}

/*
    Trying to query an invalid database (the "values" field is not an array)

    Expectation: the query returns an empty result
 */
TEST(Database, fromInvalidJsonValuesFieldNotAnArray)
{
    DatabaseMock db("test.json");
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD, "value"}};
    EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
    EXPECT_FALSE(db.find("sf2"));
}

/*
    The database contain a value with a missing key

    Expectation: that key is not added to the database
    therefore querying the key returns an empty result
*/
TEST(Database, fromInvalidJsonValueMissingKey)
{
    DatabaseMock db("test.json");
    auto invalidKey = "invalidKey";
    auto key = "key";
    auto info = "info";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {{{"invalid", invalidKey}, {DatabaseMock::INFO_JSON_FIELD, info}},
                                {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

    EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
    // This one should not be available in the database since it had an invalid key
    EXPECT_FALSE(db.find(invalidKey));
    // This one should be available instead since it had a valid key
    auto found = db.find(key);
    ASSERT_TRUE(found);
    EXPECT_EQ(*found, info);
}

/*
    The database contain a value with an invalid key (unconvertible to desired type)

    Expectation: that key is not added to the database
    therefore querying the key returns an empty result
*/
TEST(Database, fromInvalidJsonValueInvalidKey)
{
    DatabaseMock db("test.json");
    auto invalidKey = "invalidKey";
    auto key = "key";
    auto info = "info";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {{{DatabaseMock::KEY_JSON_FIELD, 0}, {DatabaseMock::INFO_JSON_FIELD, info}},
                                {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

    EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
    // This one should not be available in the database since it had an invalid key
    EXPECT_FALSE(db.find(invalidKey));
    // This one should be available instead since it had a valid key
    auto found = db.find(key);
    ASSERT_TRUE(found);
    EXPECT_EQ(*found, info);
}

/*
    The database contain a value with a missing info

    Expectation: that key is not added to the database
    therefore querying the key returns an empty result
*/
TEST(Database, fromInvalidJsonValueMissingInfo)
{
    DatabaseMock db("test.json");
    auto invalidKey = "invalidKey";
    auto invalidInfo = "invalidInfo";
    auto key = "key";
    auto info = "info";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {{{DatabaseMock::KEY_JSON_FIELD, invalidKey}, {"invalid", invalidInfo}},
                                {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

    EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
    // This one should not be available in the database since it had an invalid key
    EXPECT_FALSE(db.find(invalidKey));
    // This one should be available instead since it had a valid key
    auto found = db.find(key);
    ASSERT_TRUE(found);
    EXPECT_EQ(*found, info);
}

/*
    The database contain a value with an invalid info (unconvertible to desired type)

    Expectation: that key is not added to the database
    therefore querying the key returns an empty result
*/
TEST(Database, fromInvalidJsonValueInvalidInfo)
{
    DatabaseMock db("test.json");
    auto invalidKey = "invalidKey";
    auto key = "key";
    auto info = "info";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {{{DatabaseMock::KEY_JSON_FIELD, invalidKey}, {DatabaseMock::INFO_JSON_FIELD, 0}},
                                {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

    EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
    // This one should not be available in the database since it had an invalid key
    EXPECT_FALSE(db.find(invalidKey));
    // This one should be available instead since it had a valid key
    auto found = db.find(key);
    ASSERT_TRUE(found);
    EXPECT_EQ(*found, info);
}

/*
    Attempt a database query

    Expectation: we return the correct result if available
    an empty result if it is not available
 */
TEST(Database, queryDatabase)
{
    DatabaseMock db("test.json");
    auto key = "key";
    auto info = "info";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {{{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

    EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
    // This one should be missing from the database
    EXPECT_FALSE(db.find("missing"));
    // This one should be available instead
    auto found = db.find(key);
    ASSERT_TRUE(found);
    EXPECT_EQ(*found, info);
}
