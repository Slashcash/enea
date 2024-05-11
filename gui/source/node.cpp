#include "node.hpp"

void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Calculating new transformation
    states.transform *= getTransform();

    // Drawing this node
    drawEffective(target, states);

    // Drawing all of his children
    for (const auto& node : mNodes)
    {
        target.draw(*node, states);
    }
}

void Node::addChild(const std::shared_ptr<Node>& node)
{
    mNodes.push_back(node);
}

void Node::deleteChildren()
{
    mNodes.clear();
}
