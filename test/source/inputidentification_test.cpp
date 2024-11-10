#include "input/identification.hpp"

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

static const std::string NAME = "PS4 Controller";
static const unsigned int VENDOR_ID = 100;
static const unsigned int PRODUCT_ID = 200;
static const Input::Type TYPE = Input::Type::Joystick;

/*
    We build an Input::Identification from a json
    containing all the needed arguments.

    Expectation: Input::Identification is built with
    all its fields correctly set.
*/
TEST(InputIdentification, fromJson)
{
    Input::Identification identification(nlohmann::json({{Input::Identification::TYPE_JSON_FIELD, TYPE},
                                                         {Input::Identification::NAME_JSON_FIELD, NAME},
                                                         {Input::Identification::VENDORID_JSON_FIELD, VENDOR_ID},
                                                         {Input::Identification::PRODUCTID_JSON_FIELD, PRODUCT_ID}}));

    EXPECT_EQ(identification.type, TYPE);
    EXPECT_EQ(identification.name, NAME);
    EXPECT_EQ(identification.vendorId, VENDOR_ID);
    EXPECT_EQ(identification.productId, PRODUCT_ID);
}

/*
    We build an Input::Identification from a json
    with a missing type.

    Expectation: We throw as type is a mandatory field.
*/
TEST(InputIdentification, fromJsonMissingType)
{
    EXPECT_THROW((Input::Identification(nlohmann::json({{Input::Identification::NAME_JSON_FIELD, NAME},
                                                        {Input::Identification::VENDORID_JSON_FIELD, VENDOR_ID},
                                                        {Input::Identification::PRODUCTID_JSON_FIELD, PRODUCT_ID}}))),
                 enea::json::MissingMandatoryFieldException);
}

/*
    We build an Input::Identification from a json
    with an invalid type.

    Expectation: We throw as type is a oneof field.
*/
TEST(InputIdentification, fromJsonInvalidType)
{
    EXPECT_THROW((Input::Identification(nlohmann::json({{Input::Identification::TYPE_JSON_FIELD, "invalid"},
                                                        {Input::Identification::NAME_JSON_FIELD, NAME},
                                                        {Input::Identification::VENDORID_JSON_FIELD, VENDOR_ID},
                                                        {Input::Identification::PRODUCTID_JSON_FIELD, PRODUCT_ID}}))),
                 enea::json::CannotConvertException<Input::Type>);
}

/*
    We build an Input::Identification from a json
    with a missing name.

    Expectation: We throw as name is a mandatory field.
*/
TEST(InputIdentification, fromJsonMissingName)
{
    EXPECT_THROW((Input::Identification(nlohmann::json({{Input::Identification::TYPE_JSON_FIELD, TYPE},
                                                        {Input::Identification::VENDORID_JSON_FIELD, VENDOR_ID},
                                                        {Input::Identification::PRODUCTID_JSON_FIELD, PRODUCT_ID}}))),
                 enea::json::MissingMandatoryFieldException);
}

/*
    We build an Input::Identification from a json
    with a missing vendor id.

    Expectation: We throw as vendor id is a mandatory field.
*/
TEST(InputIdentification, fromJsonMissingVendorId)
{
    EXPECT_THROW((Input::Identification(nlohmann::json({{Input::Identification::NAME_JSON_FIELD, NAME},
                                                        {Input::Identification::TYPE_JSON_FIELD, TYPE},
                                                        {Input::Identification::PRODUCTID_JSON_FIELD, PRODUCT_ID}}))),
                 enea::json::MissingMandatoryFieldException);
}

/*
    We build an Input::Identification from a json
    with a missing product id.

    Expectation: We throw as product id is a mandatory field.
*/
TEST(InputIdentification, fromJsonMissingProductId)
{
    EXPECT_THROW((Input::Identification(nlohmann::json({{Input::Identification::NAME_JSON_FIELD, NAME},
                                                        {Input::Identification::VENDORID_JSON_FIELD, VENDOR_ID},
                                                        {Input::Identification::TYPE_JSON_FIELD, TYPE}}))),
                 enea::json::MissingMandatoryFieldException);
}

/*
    We build a json from an Input::Identification.

    Expectation: the json has all the needed fields
    correctly set.
*/
TEST(InputIdentification, toJson)
{
    nlohmann::json json(Input::Identification{TYPE, NAME, VENDOR_ID, PRODUCT_ID});

    ASSERT_TRUE(json.contains(Input::Identification::TYPE_JSON_FIELD));
    EXPECT_EQ(json[Input::Identification::TYPE_JSON_FIELD], TYPE);

    ASSERT_TRUE(json.contains(Input::Identification::NAME_JSON_FIELD));
    EXPECT_EQ(json[Input::Identification::NAME_JSON_FIELD], NAME);

    ASSERT_TRUE(json.contains(Input::Identification::VENDORID_JSON_FIELD));
    EXPECT_EQ(json[Input::Identification::VENDORID_JSON_FIELD], VENDOR_ID);

    ASSERT_TRUE(json.contains(Input::Identification::PRODUCTID_JSON_FIELD));
    EXPECT_EQ(json[Input::Identification::PRODUCTID_JSON_FIELD], PRODUCT_ID);
}

/*
    We convert an Input::Identification to a string.

    Expectation: we get a properly formatted string.
 */
TEST(InputIdentification, toString)
{
    Input::Identification identification{TYPE, NAME, VENDOR_ID, PRODUCT_ID};

    const auto expected =
        fmt::format("{}: {}. Vendor ID: {}, Product ID: {}", magic_enum::enum_name(TYPE), NAME, VENDOR_ID, PRODUCT_ID);

    EXPECT_EQ(identification.toString(), expected);
    EXPECT_EQ(fmt::format("{}", identification), expected);
}
