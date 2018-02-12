#include "node.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

Node::Node() {
    parent = nullptr;
    active = true;
    updrate = sf::Time::Zero;
}

void Node::attachChild(Node* const theChild) {
    //if it is already attached to another parent detaches it
    if( theChild->parent != nullptr ) theChild->parent->detachChild(theChild);

    theChild->parent = this;
    children.push_back(theChild);
}

bool Node::detachChild(Node* const theChild) {
    bool found = false;

    for(auto it = children.begin(); it < children.end(); it++) //iterate through every child to find it
        if( *it == theChild ) { //if founded
            theChild->parent = nullptr;
            children.erase(it);
            found = true;
        }

    return found;
}

void Node::draw(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const {
    //this rectangle is needed to evaluate if the node has to be drawn or not (if it is outside the active view, it does not get drawn)
    sf::FloatRect viewrect(theRenderTarget.getView().getCenter().x-theRenderTarget.getView().getSize().x/2, theRenderTarget.getView().getCenter().y-theRenderTarget.getView().getSize().y/2, theRenderTarget.getView().getSize().x, theRenderTarget.getView().getSize().y);

    //this node transformation gets stored and passed to the children
    theRenderStates.transform *= getTransform();

    //the node itself gets drawn only if it is inside the active view
    if( viewrect.intersects(getGlobalBounds()) ) drawCurrent(theRenderTarget, theRenderStates);

    //draw all children passing the transform
    for(auto it = children.begin(); it < children.end(); it++) (*it)->draw(theRenderTarget, theRenderStates);
}

void Node::update(const sf::Time theElapsedTime, const sf::FloatRect& theUpdRect) {
    //calculates if the updrect is all zeroes
    bool updrectvalid = true;
    if( theUpdRect.left == 0 && theUpdRect.top == 0 && theUpdRect.width == 0 && theUpdRect.height == 0 ) updrectvalid = false;

    //updates the elapsed time
    elap = elap + theElapsedTime;
    if( (active) && (elap >= updrate) && (!updrectvalid || theUpdRect.intersects(getLocalBounds()) ) ) { //it gets updated only if it is active and enough time has passed
        elap = sf::Time::Zero;
        updateCurrent();
    }

    //update all children
    for(auto it = children.begin(); it < children.end(); it++) (*it)->update(theElapsedTime, theUpdRect);
}

sf::Vector2f Node::getGlobalPosition() const {
    sf::FloatRect rect = getTotalRect();

    return sf::Vector2f(rect.left, rect.top);
}

const sf::Transform Node::getTotalTransform() const {
    //recursive function that calculates the total transformation chain
    if( parent == nullptr ) return sf::Transform::Identity;
    else return parent->getTransform() * parent->getTotalTransform();
}

const sf::FloatRect Node::getTotalRect() const {
    //translates the node bounding box into the totally transformed coordinate system
    return getTotalTransform().transformRect(getLocalBounds());
}
