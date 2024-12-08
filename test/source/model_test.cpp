#include "model.hpp"

#include <gtest/gtest.h>

/*
    Constructing the model element.
    Expectation: It should be correctly formed.
*/
TEST(ModelElement, construct)
{
    const std::string TEST_STRING = "test";
    ModelElement element(TEST_STRING);

    ASSERT_EQ(*element, TEST_STRING);
    ASSERT_FALSE(element.uuid().is_nil());
}

/*
    A new value is assigned to the model element.
    Expectation: the model element should contain the new value.
*/
TEST(ModelElement, reassign)
{
    const std::string TEST_STRING = "test";
    ModelElement<std::string> element("lol");
    element.reset(TEST_STRING);

    ASSERT_EQ(*element, TEST_STRING);
}

/*
    Adding a new value to the model.
    Expectation: the model contains the new value
*/
TEST(Model, add)
{
    const std::string TEST_STRING = "test";

    Model<std::string> model;
    model.addElement(TEST_STRING);
    auto elements = model.elements();
    ASSERT_EQ(elements.size(), 1);
    ASSERT_EQ(*(elements.begin()), TEST_STRING);
}
