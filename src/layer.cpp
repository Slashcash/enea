#include "layer.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

void Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.setView(view); //modifies the target and sets the correct view
    for( auto it = nodes.begin(); it < nodes.end(); it++ ) target.draw(*(*it), states); //draws all the nodes
    target.setView(target.getDefaultView()); //sets back the default view
}

bool Layer::detach(Node* const theChild) {
    bool found = false;

    for(auto it = nodes.begin(); it < nodes.end(); it++) //iterate through every node to find it
        if( *it == theChild ) { //if founded
            nodes.erase(it);
            found = true;
    }

    return found;
}

void Layer::attach(Node* const theChild) {
    //only an orphan node can be attached to a layer
    if( theChild->parent != nullptr ) theChild->parent->detachChild(theChild);

    nodes.push_back(theChild);
}

void Layer::update(const sf::Time theTimeElapsed, const sf::FloatRect theUpdRect) {
    for( auto it = nodes.begin(); it < nodes.end(); it++ ) (*it)->update(theTimeElapsed, theUpdRect); //updates every node attached to this layer
}
