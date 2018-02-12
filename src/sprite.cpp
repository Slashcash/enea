#include "sprite.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

void Sprite::drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const {
    theRenderTarget.draw(sprite, theRenderStates);
}

