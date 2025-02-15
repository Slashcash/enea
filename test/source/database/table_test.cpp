#include <gtest/gtest.h>

#include "database/table_mock.hpp"

using Key = std::string;
using Value = std::string;
using TableMock = Database::VTableMock<Key, Value>;

const Key KEY = "sf2";
const Value VALUE = "Street Fighter II";

[[nodiscard]] inline TableMock::ReadResult readSuccessFromJson(const nlohmann::json& json)
{
    return TableMock::ReadResult::Right(json.dump());
}

struct VTableTestParameter
{
    TableMock::ReadResult readResult;
    Database::Error loadResult;
    TableMock::QueryResult queryResult;
};

class VTableTest : public ::testing::TestWithParam<VTableTestParameter>
{
 protected:
    TableMock table;

    /**
     * This helper function is used to easily load a database table within the context of these unit tests.
     * It performs the operation and also checks that:
     *
     * - The table results as unloaded before attempting to load
     * - It contains no record before attempting to load
     * - Attempting a query before the load operation returns an error
     * - The load operation returns the expected value
     * - Retriggering the load operation twice does not really re-read the database file
     */
    inline void loadDatabase(const TableMock::ReadResult& readOperationResult)
    {
        ON_CALL(table, readFromFile()).WillByDefault(testing::Return(readOperationResult));
        EXPECT_CALL(table, readFromFile()).Times(testing::Exactly(1));

        EXPECT_FALSE(table.isLoaded());
        EXPECT_EQ(table.find(KEY), TableMock::queryFailed(Database::Result::NOT_LOADED));

        EXPECT_EQ(table.load(), GetParam().loadResult);
        EXPECT_EQ(table.load(), GetParam().loadResult);
        EXPECT_TRUE(table.isLoaded());
    }
};

TEST_P(VTableTest, query)
{
    loadDatabase(GetParam().readResult);
    EXPECT_EQ(table.find(KEY), GetParam().queryResult);
}

// clang-format off

/**
 * Read a perfectly coherent json from file.
 *
 * Expectations:
 * - The load operation is successful
 * - The query operation for an existing record reports a valid result
 */
INSTANTIATE_TEST_SUITE_P(success, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json{{TableMock::VALUES_JSON_FIELD,
        {
            {{TableMock::KEY_JSON_FIELD, KEY}, {TableMock::VALUE_JSON_FIELD, VALUE}},
        }}})
    },
.loadResult
    {Database::Error(Database::Result::SUCCESS)},
.queryResult
    {TableMock::querySuccess(VALUE)}
}));

/**
 * Read a perfectly coherent json from file.
 *
 * Expectations:
 * - The load operation is successful
 * - The query operation for a non-existing record reports a valid result
 */
INSTANTIATE_TEST_SUITE_P(nonExistingKey, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json{{TableMock::VALUES_JSON_FIELD,
        {
            {{TableMock::KEY_JSON_FIELD, "test"}, {TableMock::VALUE_JSON_FIELD, VALUE}},
        }}})
    },
.loadResult
    {Database::Error(Database::Result::SUCCESS)},
.queryResult
    {TableMock::querySuccess(std::nullopt)}
}));

/**
 * Fail to read the table json file.
 *
 * Expectations:
 * - The load operation reports a OPEN_DATABASE_FILE error
 * - The query operation reports a OPEN_DATABASE_FILE error
*/
INSTANTIATE_TEST_SUITE_P(readFailedOpenFile, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {TableMock::readFailed(Database::Result::OPEN_DATABASE_FILE)},
.loadResult
    {Database::Error(Database::Result::OPEN_DATABASE_FILE)},
.queryResult
    {TableMock::queryFailed(Database::Result::OPEN_DATABASE_FILE)}
}));

/**
 * Read an empty json from file.
 *
 * Expectations:
 * - The load operation reports a PARSE_JSON error
 * - The query operation reports a PARSE_JSON error
*/
INSTANTIATE_TEST_SUITE_P(readFailedEmptyJson, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson("")},
.loadResult
    {Database::Error(Database::Result::PARSE_JSON)},
.queryResult
    {TableMock::queryFailed(Database::Result::PARSE_JSON)}
}));

/**
 * Read an invalid json from file.
 *
 * Expectations:
 * - The load operation reports a PARSE_JSON error
 * - The query operation reports a PARSE_JSON error
*/
INSTANTIATE_TEST_SUITE_P(readFailedInvalidJson, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson("invalid")},
.loadResult
    {Database::Error(Database::Result::PARSE_JSON)},
.queryResult
    {TableMock::queryFailed(Database::Result::PARSE_JSON)}
}));

/**
 * Read a json from file that does not contain the mandatory "values" array.
 *
 * Expectations:
 * - The load operation reports a PARSE_JSON error
 * - The query operation reports a PARSE_JSON error
*/
INSTANTIATE_TEST_SUITE_P(readFailedMissingValuesArray, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json
        {{
            "invalid", "value"
        }})},
.loadResult
    {Database::Error(Database::Result::PARSE_JSON)},
.queryResult
    {TableMock::queryFailed(Database::Result::PARSE_JSON)}
}));

/**
 * Read a json from file that contains an invalid "values" array.
 *
 * Expectations:
 * - The load operation reports a PARSE_JSON error
 * - The query operation reports a PARSE_JSON error
*/
INSTANTIATE_TEST_SUITE_P(readFailedInvalidValuesArray, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json
            {{"invalid", "value"}}
        )},
.loadResult
    {Database::Error(Database::Result::PARSE_JSON)},
.queryResult
    {TableMock::queryFailed(Database::Result::PARSE_JSON)}
}));

/**
 * Read a json from file that contains a record with a missing key.
 *
 * Expectations:
 * - The load operation is successful
 * - The query operation for the malformed record reports no result
 */
INSTANTIATE_TEST_SUITE_P(readMissingKey, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json{{TableMock::VALUES_JSON_FIELD,
        {
            {{TableMock::VALUE_JSON_FIELD, VALUE}},
        }}})
    },
.loadResult
    {Database::Error(Database::Result::SUCCESS)},
.queryResult
    {TableMock::querySuccess(std::nullopt)}
}));

/**
 * Read a json from file that contains a record with an invalid key.
 *
 * Expectations:
 * - The load operation is successful
 * - The query operation for the malformed record reports no result
 */
INSTANTIATE_TEST_SUITE_P(readInvalidKey, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json{{TableMock::VALUES_JSON_FIELD,
        {
            {{TableMock::KEY_JSON_FIELD, 1}, {TableMock::VALUE_JSON_FIELD, VALUE}},
        }}})
    },
.loadResult
    {Database::Error(Database::Result::SUCCESS)},
.queryResult
    {TableMock::querySuccess(std::nullopt)}
}));

/**
 * Read a json from file that contains a record with a missing value.
 *
 * Expectations:
 * - The load operation is successful
 * - The query operation for the malformed record reports no result
 */
INSTANTIATE_TEST_SUITE_P(readMissingValue, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json{{TableMock::VALUES_JSON_FIELD,
        {
            {{TableMock::KEY_JSON_FIELD, KEY}},
        }}})
    },
.loadResult
    {Database::Error(Database::Result::SUCCESS)},
.queryResult
    {TableMock::querySuccess(std::nullopt)}
}));

/**
 * Read a json from file that contains a record with an invalid value.
 *
 * Expectations:
 * - The load operation is successful
 * - The query operation for the malformed record reports no result
 */
INSTANTIATE_TEST_SUITE_P(readInvalidValue, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json{{TableMock::VALUES_JSON_FIELD,
        {
            {{TableMock::KEY_JSON_FIELD, KEY}, {TableMock::VALUE_JSON_FIELD, 1}},
        }}})
    },
.loadResult
    {Database::Error(Database::Result::SUCCESS)},
.queryResult
    {TableMock::querySuccess(std::nullopt)}
}));

/**
 * Read a perfectly coherent json with a duplicated entry.
 *
 * Expectations:
 * - The load operation is successful
 * - The query operation for an existing record reports a valid result
 */
INSTANTIATE_TEST_SUITE_P(readDuplicatedRecord, VTableTest, ::testing::Values(VTableTestParameter{
.readResult
    {readSuccessFromJson(
        nlohmann::json{{TableMock::VALUES_JSON_FIELD,
        {
            {{TableMock::KEY_JSON_FIELD, KEY}, {TableMock::VALUE_JSON_FIELD, VALUE}},
            {{TableMock::KEY_JSON_FIELD, KEY}, {TableMock::VALUE_JSON_FIELD, VALUE}},
        }}})
    },
.loadResult
    {Database::Error(Database::Result::SUCCESS)},
.queryResult
    {TableMock::querySuccess(VALUE)}
}));
