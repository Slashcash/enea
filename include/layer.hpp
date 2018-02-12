#ifndef _LAYER_HPP_
#define _LAYER_HPP_

#include <SFML/Graphics/View.hpp>

#include "node.hpp"

/*a class that abstracts a layer in a scene, this class is not intended to be instantiated directly, it is used by the scene class
it wraps a sort of 2D Camera inside it (every layer has a camera that can be independtly be moved, rotated or zoomed, it is implemented using SFML view concept)
only orphan nodes can be attached to a layer*/

class Scene;

class Layer : public sf::Drawable {
    private:
        //members
        sf::View view;
        std::vector<Node*> nodes; //all the nodes in a layer

        //ctor
        Layer() {}
        Layer(const sf::FloatRect& theCamera) : view(theCamera) {} //this construct a layer and sets the boundaries of the 2d camera for this layer

        //getters
        const sf::Vector2f getCameraSize() const { return view.getSize(); } //gets the 2d camera size
        const sf::Vector2f getCameraCenter() const { return view.getCenter(); } //gets the 2d camera center

        //setters
        void setCamera(const sf::FloatRect& theRectangle) { view.reset(theRectangle); } //sets the 2d camera boundaries

        //functions
        void attach(Node* const theChild); //attaches a node to this layer (if it is not an orphan detaches it from parent)
        bool detach(Node* const theChild); //if child is attached to this layer, detaches it
        void moveCamera(const float offsetx, const float offsety) { view.move(offsetx, offsety); } //moves the layer camera
        void moveCamera(const sf::Vector2f offset) { moveCamera(offset.x, offset.y); }
        void rotateCamera(const float angle) { view.rotate(-angle); } //rotates the layer camera
        void zoomCamera(const float factor) { view.zoom(factor); } //zooms the layer camera
        void update(const sf::Time theTimeElapsed, const sf::FloatRect theUpdRect = sf::FloatRect(0, 0, 0, 0)); //updates every node attached to this layer
        virtual void draw(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const; //draws every node attached to this layer

        friend class Scene;
};


#endif // _LAYER_HPP_
