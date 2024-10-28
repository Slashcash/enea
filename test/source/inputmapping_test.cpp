#include "inputmapping.hpp"

#include <gtest/gtest.h>
#include <magic_enum.hpp>

#include "inputbutton.hpp"

static const auto BUTTON_TYPE = Input::Button::Type::joystick_digital;
static const auto BUTTON_DEFINITION = "x";
static const auto EMULATOR_COMMAND_UP = Input::Emulator::Command::p1_up;
static const auto EMULATOR_COMMAND_DOWN = Input::Emulator::Command::p1_down;
static const Input::Button BUTTON{BUTTON_TYPE, BUTTON_DEFINITION};

TEST(InputMapping, fromConstructor)
{
    /*
        Build a Mapping from an user provided map
        Expectation: the Mapping is correctly built and we can query
        for available buttons
    */
    Input::Mapping mapping({{EMULATOR_COMMAND_DOWN, BUTTON}, {EMULATOR_COMMAND_UP, BUTTON}});

    const auto commandUp = mapping.getButton(EMULATOR_COMMAND_UP);
    EXPECT_TRUE(commandUp && *commandUp == BUTTON);

    const auto commandDown = mapping.getButton(EMULATOR_COMMAND_DOWN);
    EXPECT_TRUE(commandDown && *commandDown == BUTTON);

    EXPECT_FALSE(mapping.getButton(Input::Emulator::Command::coin1));
}

TEST(InputMapping, toJson)
{
    /*
        Convert a mapping to a json
        Expectation: we get a correctly mapped json
     */
    nlohmann::json json = Input::Mapping({{EMULATOR_COMMAND_UP, BUTTON}});
    ASSERT_TRUE(json.contains(Input::Mapping::MAPPING_JSON_FIELD));
    auto mappings = json[Input::Mapping::MAPPING_JSON_FIELD];
    EXPECT_TRUE(mappings.contains(magic_enum::enum_name(EMULATOR_COMMAND_UP)) &&
                Input::Button(mappings[magic_enum::enum_name(EMULATOR_COMMAND_UP)]) == BUTTON);
}

TEST(InputMapping, fromJson)
{
    /*
        Build a Mapping from a json
        Expectation: the Mapping is correctly built and we can query
        for available buttons
    */
    Input::Mapping mapping = nlohmann::json{{Input::Mapping::MAPPING_JSON_FIELD,
                                             {{magic_enum::enum_name(EMULATOR_COMMAND_UP), BUTTON},
                                              {magic_enum::enum_name(EMULATOR_COMMAND_DOWN), BUTTON}}}};

    const auto commandUp = mapping.getButton(EMULATOR_COMMAND_UP);
    EXPECT_TRUE(commandUp && *commandUp == BUTTON);

    const auto commandDown = mapping.getButton(EMULATOR_COMMAND_DOWN);
    EXPECT_TRUE(commandDown && *commandDown == BUTTON);

    EXPECT_FALSE(mapping.getButton(Input::Emulator::Command::coin1));
}

TEST(InputMapping, fromJsonInvalidCommand)
{
    /*
        Build a Mapping from a json with a missing mandatory field (the command)
        Expectation: we throw
    */
    EXPECT_THROW((Input::Mapping{
                     nlohmann::json{{Input::Mapping::MAPPING_JSON_FIELD,
                                     {{"invalid", BUTTON}, {magic_enum::enum_name(EMULATOR_COMMAND_DOWN), BUTTON}}}}}),
                 Exception);
}
