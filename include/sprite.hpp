#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "node.hpp"


/*more or less a wrapper around SFML sprite*/

class Sprite : public Node {
    private:
        //members
        sf::Sprite sprite;

        //functions
        virtual void drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const;
        virtual float getWidth() const { return sprite.getLocalBounds().width; }
        virtual float getHeight() const { return sprite.getLocalBounds().height; }

    public:
        //ctor
        Sprite() {}
        Sprite(const sf::Texture& theTexture) { sprite.setTexture(theTexture); } //BE AWARE THAT ONLY THE REFERENCE TO THE TEXTURE IS STORED, TEXTURE LIFETIME HAS TO BE MANAGED EXTERNALLY

        //setters
        void setTexture(const sf::Texture& theTexture, const bool resetRect = false) { sprite.setTexture(theTexture, resetRect); } //BE AWARE THAT ONLY THE REFERENCE TO THE TEXTURE IS STORED, TEXTURE LIFETIME HAS TO BE MANAGED EXTERNALLY
};

#endif // _SPRITE_HPP_
