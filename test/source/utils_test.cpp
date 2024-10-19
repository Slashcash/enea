#include "utils.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

TEST(Utils, addOptionalToJson)
{
    /*
        Add an std::optional containing a value in the json.
        Expectation: the json will contain the added value
    */
    nlohmann::json json;
    const std::string_view FIELD = "field";
    const std::string VALUE = "value";
    utils::addOptionalToJson(json, FIELD, std::optional<std::string>(VALUE));

    ASSERT_TRUE(json.contains(FIELD));
    EXPECT_EQ(json[FIELD], VALUE);
}

TEST(Utils, addEmptyOptionalToJson)
{
    /*
        Add an empty std::optional in the json.
        Expectation: the json will not contain the added value
    */
    nlohmann::json json;
    const std::string_view FIELD = "field";
    utils::addOptionalToJson(json, FIELD, std::optional<std::string>());

    ASSERT_FALSE(json.contains(FIELD));
}

TEST(Utils, getValueFromJson)
{
    /*
        Get a value available in the json
        Expectation: the value will be correctly returned
    */
    const std::string_view FIELD = "field";
    const std::string VALUE = "value";
    nlohmann::json json{{FIELD, VALUE}};
    auto optValue = utils::getValueFromJson<std::string>(json, FIELD);
    ASSERT_TRUE(optValue);
    ASSERT_EQ(*optValue, VALUE);
}

TEST(Utils, getEmptyValueFromJson)
{
    /*
        Get a value unavailable in the json
        Expectation: an empty value will be returned
    */
    nlohmann::json json;
    auto optValue = utils::getValueFromJson<std::string>(json, "field");
    ASSERT_FALSE(optValue);
}
