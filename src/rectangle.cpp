#include "rectangle.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

void Rectangle::drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const {
    theRenderTarget.draw(rectangle, theRenderStates);
}
