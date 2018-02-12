#include "scene.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

void Scene::attachToLayer(Node* const child, const unsigned int layernum) {
    if( layernum >= layers.size() ) //if asking to add to a non existent layer...
        for( unsigned int i = layers.size(); i <= layernum; i++ )
            addNewLayer(); //add the layers until it exists...

    layers[layernum].attach(child); //then attaches the child correctly
}

bool Scene::detachFromLayer(Node* const child, const unsigned int layernum) {
    //if asking to remove from a non existent layer, don't do anything
    if( layernum >= layers.size() ) return false;

    else return layers[layernum].detach(child);
}

void Scene::addNewLayer() {
    layers.push_back(Layer(sf::FloatRect(0, 0, cameradefaultsize.x, cameradefaultsize.y))); //adds a new layer with the DEFAULT camera size
}

void Scene::moveLayer(const float offsetx, const float offsety, const unsigned int layernum) {
    if( layernum < layers.size() ) layers[layernum].moveCamera(offsetx, offsety);
}

void Scene::zoomLayer(const float factor, const unsigned int layernum) {
    if( layernum < layers.size() ) layers[layernum].zoomCamera(factor);
}

void Scene::rotateLayer(const float angle, const unsigned int layernum) {
    if( layernum < layers.size() ) layers[layernum].rotateCamera(angle);
}

void Scene::move(const float offsetx, const float offsety) {
    for(auto it = layers.begin(); it < layers.end(); it++) (*it).moveCamera(offsetx, offsety);
}

void Scene::moveParallax(const float offsetx, const float offsety, std::vector<sf::Vector2f> scrollfactors) {
    if( scrollfactors.size() == layers.size() )
        for(unsigned int i = 0; i < layers.size(); i++)
            moveLayer(offsetx*scrollfactors[i].x, offsety*scrollfactors[i].y, i);
}

void Scene::zoom(const float factor) {
    for(auto it = layers.begin(); it < layers.end(); it++) (*it).zoomCamera(factor);
}

void Scene::rotate(const float angle) {
    for(auto it = layers.begin(); it < layers.end(); it++) (*it).rotateCamera(angle);
}

void Scene::draw(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const {
    for(auto it = layers.begin(); it < layers.end(); it++)
        theRenderTarget.draw(*it, theRenderStates);
}

void Scene::update(sf::Time elapsed) {
    for(auto it = layers.begin(); it < layers.end(); it++) (*it).update(elapsed, sf::FloatRect(0, 0, scenesize.x, scenesize.y));
}
