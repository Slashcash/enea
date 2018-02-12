#ifndef _RECTANGLE_HPP_
#define _RECTANGLE_HPP_

/*This class is more or less a wrapper around sfml rectangleshape*/

#include <SFML/Graphics/RectangleShape.hpp>

#include "node.hpp"

class Rectangle : public Node {
    private:
        //members
        sf::RectangleShape rectangle;

        //functions
        virtual float getWidth() const { return rectangle.getLocalBounds().width; }
        virtual float getHeight() const { return rectangle.getLocalBounds().height; }
        virtual void drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const;

    public:
        //ctor
        Rectangle() = default;

        //getters
        sf::Color getFillColor() const { return rectangle.getFillColor(); }
        sf::Color getOutlineColor() const { return rectangle.getOutlineColor(); }
        float getOutlineThickness() const { return rectangle.getOutlineThickness(); }

        //setters
        void setTexture(const sf::Texture& theTexture) { rectangle.setTexture(&theTexture); }
        void setFillColor(const sf::Color& theColor) { rectangle.setFillColor(theColor); }
        void setOutlineColor(const sf::Color& theColor) { rectangle.setOutlineColor(theColor); }
        void setOutlineThickness(const float theThickness) { rectangle.setOutlineThickness(theThickness); }
        void setSize(const sf::Vector2f& theSize) { rectangle.setSize(theSize); }
};

#endif // _RECTANGLE_HPP_
