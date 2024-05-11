#ifndef NODE_HPP
#define NODE_HPP

#include <list>
#include <memory>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>

class Node : public sf::Drawable, public sf::Transformable
{
 private:
    std::list<std::shared_ptr<Node>> mNodes;

    virtual void drawEffective(sf::RenderTarget& target, sf::RenderStates states) const = 0;

 public:
    Node() = default;
    Node(const Node& node) = delete;
    Node(Node&& node) = delete;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void addChild(const std::shared_ptr<Node>& node);

    Node& operator=(const Node& node) = delete;
    Node& operator=(Node&& node) = delete;

    void deleteChildren();

    ~Node() override = default;
};

template <typename F>
concept SFMLDrawable = requires(F drawable, sf::RenderTarget& target, sf::RenderStates states) {
    requires std::is_default_constructible_v<F>;
    { target.draw(drawable, states) } -> std::same_as<void>;
};

template <SFMLDrawable T> class SFMLNode : public Node
{
 private:
    T mElement;

    void inline drawEffective(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(mElement, states);
    }

 public:
    [[nodiscard]] inline T& element()
    {
        return mElement;
    }
};

using TextNode = SFMLNode<sf::Text>;
using SpriteNode = SFMLNode<sf::Sprite>;

#endif // NODE_HPP
