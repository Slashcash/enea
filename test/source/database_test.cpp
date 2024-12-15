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

// /*
//     Trying to query a database that was not correctly loaded
//     from file.

//     Expectation: the query returns an empty result
//  */
// TEST(Database, fromInvalidFile)
// {
//     DatabaseMock db("test.json");
//     EXPECT_CALL(db, load()).WillOnce(testing::Return(std::nullopt));
//     EXPECT_FALSE(db.find("sf2"));

//     // Re-querying again on an unloaded database should return an empty result
//     // without an useless load retry
//     EXPECT_CALL(db, load()).Times(0);
//     EXPECT_FALSE(db.find("sf2"));
// }

// /*
//     Trying to query an invalid database (the "values" field is missing)

//     Expectation: the query returns an empty result
//  */
// TEST(Database, fromInvalidJsonMissingValuesArray)
// {
//     DatabaseMock db("test.json");
//     const nlohmann::json json{{"invalid", "value"}};
//     EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
//     EXPECT_FALSE(db.find("sf2"));

//     // Re-querying again on an unloaded database should return an empty result
//     // without an useless load retry
//     EXPECT_CALL(db, load()).Times(0);
//     EXPECT_FALSE(db.find("sf2"));
// }

// /*
//     Trying to query an invalid database (the "values" field is not an array)

//     Expectation: the query returns an empty result
//  */
// TEST(Database, fromInvalidJsonValuesFieldNotAnArray)
// {
//     DatabaseMock db("test.json");
//     const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD, "value"}};
//     EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
//     EXPECT_FALSE(db.find("sf2"));
// }

// /*
//     The database contain a value with a missing key

//     Expectation: that key is not added to the database
//     therefore querying the key returns an empty result
// */
// TEST(Database, fromInvalidJsonValueMissingKey)
// {
//     DatabaseMock db("test.json");
//     auto invalidKey = "invalidKey";
//     auto key = "key";
//     auto info = "info";
//     const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
//                                {{{"invalid", invalidKey}, {DatabaseMock::INFO_JSON_FIELD, info}},
//                                 {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

//     EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
//     // This one should not be available in the database since it had an invalid key
//     EXPECT_FALSE(db.find(invalidKey));
//     // This one should be available instead since it had a valid key
//     auto found = db.find(key);
//     ASSERT_TRUE(found);
//     EXPECT_EQ(*found, info);
// }

// /*
//     The database contain a value with an invalid key (unconvertible to desired type)

//     Expectation: that key is not added to the database
//     therefore querying the key returns an empty result
// */
// TEST(Database, fromInvalidJsonValueInvalidKey)
// {
//     DatabaseMock db("test.json");
//     auto invalidKey = "invalidKey";
//     auto key = "key";
//     auto info = "info";
//     const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
//                                {{{DatabaseMock::KEY_JSON_FIELD, 0}, {DatabaseMock::INFO_JSON_FIELD, info}},
//                                 {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

//     EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
//     // This one should not be available in the database since it had an invalid key
//     EXPECT_FALSE(db.find(invalidKey));
//     // This one should be available instead since it had a valid key
//     auto found = db.find(key);
//     ASSERT_TRUE(found);
//     EXPECT_EQ(*found, info);
// }

// /*
//     The database contain a value with a missing info

//     Expectation: that key is not added to the database
//     therefore querying the key returns an empty result
// */
// TEST(Database, fromInvalidJsonValueMissingInfo)
// {
//     DatabaseMock db("test.json");
//     auto invalidKey = "invalidKey";
//     auto invalidInfo = "invalidInfo";
//     auto key = "key";
//     auto info = "info";
//     const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
//                                {{{DatabaseMock::KEY_JSON_FIELD, invalidKey}, {"invalid", invalidInfo}},
//                                 {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

//     EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
//     // This one should not be available in the database since it had an invalid key
//     EXPECT_FALSE(db.find(invalidKey));
//     // This one should be available instead since it had a valid key
//     auto found = db.find(key);
//     ASSERT_TRUE(found);
//     EXPECT_EQ(*found, info);
// }

// /*
//     The database contain a value with an invalid info (unconvertible to desired type)

//     Expectation: that key is not added to the database
//     therefore querying the key returns an empty result
// */
// TEST(Database, fromInvalidJsonValueInvalidInfo)
// {
//     DatabaseMock db("test.json");
//     auto invalidKey = "invalidKey";
//     auto key = "key";
//     auto info = "info";
//     const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
//                                {{{DatabaseMock::KEY_JSON_FIELD, invalidKey}, {DatabaseMock::INFO_JSON_FIELD, 0}},
//                                 {{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

//     EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
//     // This one should not be available in the database since it had an invalid key
//     EXPECT_FALSE(db.find(invalidKey));
//     // This one should be available instead since it had a valid key
//     auto found = db.find(key);
//     ASSERT_TRUE(found);
//     EXPECT_EQ(*found, info);
// }

// /*
//     Attempt a database query

//     Expectation: we return the correct result if available
//     an empty result if it is not available
//  */
// TEST(Database, queryDatabase)
// {
//     DatabaseMock db("test.json");
//     auto key = "key";
//     auto info = "info";
//     const nlohmann::json json{{DatabaseMock::VALUES_JSON_FIELD,
//                                {{{DatabaseMock::KEY_JSON_FIELD, key}, {DatabaseMock::INFO_JSON_FIELD, info}}}}};

//     EXPECT_CALL(db, load()).WillOnce(testing::Return(json));
//     // This one should be missing from the database
//     EXPECT_FALSE(db.find("missing"));
//     // This one should be available instead
//     auto found = db.find(key);
//     ASSERT_TRUE(found);
//     EXPECT_EQ(*found, info);
// }
