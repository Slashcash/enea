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

TEST(Utils, getOptionalValueFromJson)
{
    /*
        Get a value available in the json
        Expectation: the value will be correctly returned
    */
    const std::string_view FIELD = "field";
    const std::string VALUE = "value";
    nlohmann::json json{{FIELD, VALUE}};
    auto optValue = utils::getOptionalValueFromJson<std::string>(json, FIELD);
    ASSERT_TRUE(optValue);
    ASSERT_EQ(*optValue, VALUE);
}

TEST(Utils, enumJsonConversion)
{
    enum class TEST_ENUM
    {
        test1,
        test2
    };

    TEST_ENUM testEnum = TEST_ENUM::test1;

    nlohmann::json json(testEnum);

    EXPECT_EQ(json.template get<TEST_ENUM>(), TEST_ENUM::test1);
}

TEST(Utils, getEmptyValueFromJson)
{
    /*
        Get a value unavailable in the json
        Expectation: an empty value will be returned
    */
    nlohmann::json json;
    EXPECT_THROW(utils::getOptionalValueFromJson<std::string>(json, "field"), nlohmann::json::exception);
}

TEST(Utils, getMandatoryValueFromJson)
{
    /*
        Get a mandatory value available in the json
        Expectation: the value is correctly returned
    */
    const std::string_view FIELD = "field";
    const std::string VALUE = "value";
    nlohmann::json json{{FIELD, VALUE}};

    ASSERT_EQ(utils::getMandatoryValueFromJson<std::string>(json, FIELD), VALUE);
}

TEST(Utils, getMandatoryValueFromEmptyJson)
{
    /*
        Get a mandatory value in an empty json
        Expectation: we throw
    */
    nlohmann::json json;

    ASSERT_THROW(utils::getMandatoryValueFromJson<std::string>(json, "field"),
                 enea::json::MissingMandatoryFieldException);
}

TEST(Utils, getUnavailableMandatoryValueFromJson)
{
    /*
        Get a mandatory value which is unavailable in the json
        Expectation: we throw
    */
    nlohmann::json json{"invalid", "value"};

    ASSERT_THROW(utils::getMandatoryValueFromJson<std::string>(json, "field"),
                 enea::json::MissingMandatoryFieldException);
}

TEST(Utils, getMandatoryValueFromJsonCannotConvert)
{
    /*
        Get a mandatory value which is available in the json but
        cannot be converted to the requested value
        Expectation: we throw
    */
    const std::string_view FIELD = "field";
    nlohmann::json json{{FIELD, "value"}};

    ASSERT_THROW(utils::getMandatoryValueFromJson<unsigned int>(json, FIELD),
                 enea::json::CannotConvertException<unsigned int>);
}
