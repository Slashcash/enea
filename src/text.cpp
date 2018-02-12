#include "text.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

void Text::drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const {
    theRenderTarget.draw(text, theRenderStates);
}
