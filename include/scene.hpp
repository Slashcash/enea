#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "layer.hpp"

/*a scene, implemented as a vector of layers
attach nodes to a layer and draw the scene for fantastic results
URGENT TODO: since i have imported this class from another project a lot of the functions are not camelCase and i hate this but too lazy to correct it*/

class Scene : public sf::Drawable {
    private:
        //members
        std::vector<Layer> layers;
        sf::Vector2f cameradefaultsize;
        sf::Vector2f scenesize;

        //functions
        void addNewLayer(); //adds a new layer to this scene
        virtual void draw(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const; //draws this scene

    public:
        //ctor
        Scene(const sf::Vector2f theSceneSize = sf::Vector2f(1000, 1000), const sf::Vector2f& theCameraSize = sf::Vector2f(1000, 1000)) { setSize(theSceneSize); setDefaultCameraSize(theCameraSize); }

        //getters
        sf::Vector2f getDefaultCameraSize() const { return cameradefaultsize; }
        sf::Vector2f getSize() const { return scenesize; }

        //setters
        void setDefaultCameraSize(const sf::Vector2f& theSize) { cameradefaultsize = theSize; }
        void setSize(const sf::Vector2f& theSize) { scenesize = theSize; }

        //functions
        void attachToLayer(Node* const child, const unsigned int layernum);
        bool detachFromLayer(Node* const child, const unsigned int layernum);
        void moveLayer(const float offsetx, const float offsety, const unsigned int layernum);
        void moveLayer(const sf::Vector2f& offset, const unsigned int layernum) { moveLayer(offset.x, offset.y, layernum); }
        void move(const float offsetx, const float offsety);
        void move(const sf::Vector2f& offset) { move(offset.x, offset.y); }
        void moveParallax(const float offsetx, const float offsety, std::vector<sf::Vector2f> scrollfactors); //this function implements a parallax scrolling, the n-th layer moves by a value of OFFSET*velocities[n]
        void zoomLayer(const float factor, const unsigned int layernum);
        void zoom(const float factor);
        void rotateLayer(const float angle, const unsigned int layernum);
        void rotate(const float angle);
        void update(const sf::Time elapsed);
};


#endif // _SCENE_HPP_
