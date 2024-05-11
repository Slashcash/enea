#ifndef NODEMOCK_HPP
#define NODEMOCK_HPP

#include "node.hpp"

#include <gmock/gmock.h>

class NodeMock : public Node
{
 public:
    using Node::Node;

    MOCK_METHOD(void, drawEffective, (sf::RenderTarget & target, sf::RenderStates states), (const override));
};

#endif // NODEMOCK_HPP
