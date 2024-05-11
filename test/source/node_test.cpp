#include "node_mock.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

class NodeFixture : public ::testing::Test
{
 protected:
    NodeMock node;
    std::shared_ptr<NodeMock> child;
    sf::RenderWindow window;

 public:
    inline NodeFixture()
    {
        child = std::make_shared<NodeMock>();
    }
};

TEST_F(NodeFixture, draw)
{
    // With no children added we do not draw any child, only the node
    EXPECT_CALL(node, drawEffective(::testing::_, ::testing::_)).WillOnce(testing::Return());
    EXPECT_CALL(*child, drawEffective(::testing::_, ::testing::_)).Times(0);
    window.draw(node);

    // By adding the child we draw them both
    node.addChild(child);
    EXPECT_CALL(node, drawEffective(::testing::_, ::testing::_)).WillOnce(testing::Return());
    EXPECT_CALL(*child, drawEffective(::testing::_, ::testing::_)).WillOnce(testing::Return());
    window.draw(node);

    // By removing children we go back to only drawing the node
    node.deleteChildren();
    EXPECT_CALL(node, drawEffective(::testing::_, ::testing::_)).WillOnce(testing::Return());
    EXPECT_CALL(*child, drawEffective(::testing::_, ::testing::_)).Times(0);
    window.draw(node);
}
