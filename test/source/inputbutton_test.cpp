#include "exception.hpp"
#include "input/button.hpp"

#include <gtest/gtest.h>

static const std::string STR_TYPE = "joystick_digital";
static const Input::Emulator::Button::Type TYPE = Input::Emulator::Button::Type::joystick_digital;
static const std::string DEFINITION = "select";

TEST(InputButton, fromJsonComplete)
{
    /*
        Construct a button from a complete json
        Expectation: the button struct is correctly built
    */
    Input::Emulator::Button button(nlohmann::json{{Input::Emulator::Button::TYPE_JSON_FIELD, STR_TYPE},
                                                  {Input::Emulator::Button::DEFINITION_JSON_FIELD, DEFINITION}});

    EXPECT_EQ(button.type, TYPE);
    EXPECT_EQ(button.definition, DEFINITION);
}

TEST(InputButton, fromJsonInvalidType)
{
    /*
        Construct a button from a complete json but with an invalid type
        Expectation: we throw
    */
    EXPECT_THROW(
        (Input::Emulator::Button(nlohmann::json{{Input::Emulator::Button::TYPE_JSON_FIELD, "invalid"},
                                                {Input::Emulator::Button::DEFINITION_JSON_FIELD, DEFINITION}})),
        enea::Exception);
}

TEST(InputButton, fromJsonMissingType)
{
    /*
        Construct a button from a json with missing type
        Expectation: we throw
    */
    EXPECT_THROW(
        (Input::Emulator::Button(nlohmann::json{{Input::Emulator::Button::DEFINITION_JSON_FIELD, DEFINITION}})),
        enea::Exception);
}

TEST(InputButton, fromJsonMissingDefinition)
{
    /*
        Construct a button from a json with missing definition
        Expectation: we throw
    */
    EXPECT_THROW((Input::Emulator::Button(nlohmann::json{{Input::Emulator::Button::TYPE_JSON_FIELD, STR_TYPE}})),
                 enea::Exception);
}

TEST(InputButton, toJson)
{
    /*
        Construct a json from a button
        Expectation: the json is correctly constructed
    */
    nlohmann::json json = Input::Emulator::Button{TYPE, DEFINITION};

    EXPECT_TRUE(json.contains(Input::Emulator::Button::TYPE_JSON_FIELD) &&
                json[Input::Emulator::Button::TYPE_JSON_FIELD] == STR_TYPE);
    EXPECT_TRUE(json.contains(Input::Emulator::Button::DEFINITION_JSON_FIELD) &&
                json[Input::Emulator::Button::DEFINITION_JSON_FIELD] == DEFINITION);
}
