#include "database_mock.hpp"

#include <gtest/gtest.h>

static const std::string DB_NAME = "test.json";

/*
    Trying to query a database, we load it from file but reading the file fails
    Expectation: we throw as this should never happen (database files are embedded into our executable)
*/
TEST(Database, readFailure)
{
    DatabaseMock db(DB_NAME);
    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(
            ChefFun::Either<DatabaseMock::Error, std::string>::Left(DatabaseMock::Error::OPEN_DATABASE_FILE)));

    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

/*
    Trying to query a database, we load it from file but the file is empty
    Expectation: we throw as this should never happen (database files are embedded into our executable)
*/
TEST(Database, readEmptyFile)
{
    DatabaseMock db(DB_NAME);
    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right("")));

    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

/*
    Trying to query a database, we load it from file but the file is not a valid json
    Expectation: we throw as this should never happen (database files are embedded into our executable)
*/
TEST(Database, readInvalidFile)
{
    DatabaseMock db(DB_NAME);
    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right("invalid")));

    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

/*
    Trying to query a database, we parse the json but the mandatory "value" is missing
    Expectation: we throw as this should never happen (database files are embedded into our executable)
*/
TEST(Database, jsonMissingValues)
{
    DatabaseMock db(DB_NAME);
    const nlohmann::json json{{"invalid", "value"}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

/*
    Trying to query a database, we parse the json but the mandatory "value" is invalid
    Expectation: we throw as this should never happen (database files are embedded into our executable)
*/
TEST(Database, jsonInvalidValues)
{
    DatabaseMock db(DB_NAME);
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD, "value"}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_THROW(db.find("sf2"), DatabaseMock::Exception);
}

/*
    Trying to query a database. we parse the json but one of the database entries has a missing key
    Expectation: that entry will not be added to the database
*/
TEST(Database, entryMissesKey)
{
    DatabaseMock db(DB_NAME);
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {
                                   {{DatabaseMock::INFO_JSON_FIELD, "info"}},
                               }}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 0);
}

/*
   Trying to query a database. we parse the json but one of the database entries has an invalid key (unconvertible to
   desired type)
   Expectation: that entry will not be added to the database
*/
TEST(Database, entryInvalidKey)
{
    DatabaseMock db(DB_NAME);

    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {
                                   {{DatabaseMock::KEY_JSON_FIELD, 0}, {DatabaseMock::INFO_JSON_FIELD, "info"}},
                               }}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 0);
}

/*
    Trying to query a database. we parse the json but one of the database entries has a missing info
    Expectation: that entry will not be added to the database, querying for its associated key will return no result
*/
TEST(Database, entryMissesInfo)
{
    DatabaseMock db(DB_NAME);

    auto key = "key";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD, {{{DatabaseMock::KEY_JSON_FIELD, key}}}}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 0);
    EXPECT_FALSE(db.find(key));
}

/*
   Trying to query a database. we parse the json but one of the database entries has an invalid info (unconvertible to
   desired type)
   Expectation: that entry will not be added to the database, querying for its associated key will return
   no result
*/
TEST(Database, entryInvalidInfo)
{
    DatabaseMock db(DB_NAME);
    auto key = "key";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {
                                   {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, 0}},
                               }}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 0);
    EXPECT_FALSE(db.find(key));
}

/*
   Trying to query a database. we parse the json but there is a duplicated database entry
   Expectation: that entry will only be added once to the database
*/
TEST(Database, entryDoubleInsertion)
{
    DatabaseMock db(DB_NAME);
    auto key = "key";
    auto value = "value";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {{{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, value}},
                                {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, "value2"}}}}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 1);
    auto element = db.find(key);
    ASSERT_TRUE(element);
    ASSERT_EQ(*element, value);
}

/*
    Trying to query a correctly loaded database
    Expectation: the query returns the correct result
*/
TEST(Database, query)
{
    DatabaseMock db(DB_NAME);
    auto key = "key";
    auto info = "info";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {
                                   {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}},
                               }}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 1);
    auto found = db.find(key);
    ASSERT_TRUE(found);
    EXPECT_TRUE(*found == info);
    EXPECT_FALSE(db.find("missing"));
}

/*
    Trying to query a correctly loaded database multiple times
    Expectation: the query returns the correct result everytime
*/
TEST(Database, multipleQueries)
{
    DatabaseMock db(DB_NAME);
    auto key = "key";
    auto info = "info";
    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {
                                   {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}},
                               }}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 1);

    for (unsigned int i = 0; i < 3; i++)
    {
        auto found = db.find(key);
        ASSERT_TRUE(found);
        EXPECT_TRUE(*found == info);
    }
}

/*
    Retrieving the number of entries in a correctly loaded database
    Expectation: the returned number of entries is correct
*/
TEST(Database, entries)
{
    DatabaseMock db(DB_NAME);

    auto key = "key";
    auto info = "info";

    const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
                               {
                                   {{DatabaseMock::KEY_JSON_FIELD, "1"}, {DatabaseMock::INFO_JSON_FIELD, info}},
                                   {{DatabaseMock::KEY_JSON_FIELD, "2"}, {DatabaseMock::INFO_JSON_FIELD, info}},
                                   {{DatabaseMock::KEY_JSON_FIELD, "3"}, {DatabaseMock::INFO_JSON_FIELD, info}},
                               }}};

    EXPECT_CALL(db, read(DB_NAME))
        .WillOnce(testing::Return(ChefFun::Either<DatabaseMock::Error, std::string>::Right(json.dump())));

    EXPECT_EQ(db.entries(), 3);
}
