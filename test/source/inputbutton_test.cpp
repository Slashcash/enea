#include "exception.hpp"
#include "inputbutton.hpp"

#include <gtest/gtest.h>

static const std::string STR_TYPE = "joystick_digital";
static const Input::Button::Type TYPE = Input::Button::Type::joystick_digital;
static const std::string DEFINITION = "select";

TEST(InputButton, fromJsonComplete)
{
    /*
        Construct a button from a complete json
        Expectation: the button struct is correctly built
    */
    Input::Button button(
        nlohmann::json{{Input::Button::TYPE_JSON_FIELD, STR_TYPE}, {Input::Button::DEFINITION_JSON_FIELD, DEFINITION}});

    EXPECT_EQ(button.type, TYPE);
    EXPECT_EQ(button.definition, DEFINITION);
}

TEST(InputButton, fromJsonInvalidType)
{
    /*
        Construct a button from a complete json but with an invalid type
        Expectation: we throw
    */
    EXPECT_THROW((Input::Button(nlohmann::json{{Input::Button::TYPE_JSON_FIELD, "invalid"},
                                               {Input::Button::DEFINITION_JSON_FIELD, DEFINITION}})),
                 Exception);
}

TEST(InputButton, fromJsonMissingType)
{
    /*
        Construct a button from a json with missing type
        Expectation: we throw
    */
    EXPECT_THROW((Input::Button(nlohmann::json{{Input::Button::DEFINITION_JSON_FIELD, DEFINITION}})), Exception);
}

TEST(InputButton, fromJsonMissingDefinition)
{
    /*
        Construct a button from a json with missing definition
        Expectation: we throw
    */
    EXPECT_THROW((Input::Button(nlohmann::json{{Input::Button::TYPE_JSON_FIELD, STR_TYPE}})), Exception);
}

TEST(InputButton, toJson)
{
    /*
        Construct a json from a button
        Expectation: the json is correctly constructed
    */
    nlohmann::json json = Input::Button{TYPE, DEFINITION};

    EXPECT_TRUE(json.contains(Input::Button::TYPE_JSON_FIELD) && json[Input::Button::TYPE_JSON_FIELD] == STR_TYPE);
    EXPECT_TRUE(json.contains(Input::Button::DEFINITION_JSON_FIELD) &&
                json[Input::Button::DEFINITION_JSON_FIELD] == DEFINITION);
}
