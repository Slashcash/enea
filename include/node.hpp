#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>
#include <vector>

/*the base hierarcical class for the scene, note that children's and parent's lifecycle should be manually managed
this is a pure virtual class, you need to override drawCurrent (needed for drawing the node) and getWidth/getHeight (needed for calculating the bounding box)
updating a node itself will do nothing at all, inherit from this and override updatecurrent if needed*/

class Layer;

class Node : public sf::Drawable, public sf::Transformable {
    private:
        //members
        Node* parent; //a pointer to the parent node (lifecycle to be managed manually)
        std::vector<Node*> children; //a vector of pointers to children (lifecycle manually managed)
        bool active; //setting this will enable/disable the update function
        sf::Time updrate; //this value determines the lapse between two update calls
        sf::Time elap; //internal usage (time elapsed from the last effective update call)

        // pure virtual functions
        virtual float getWidth() const = 0; //override this (needed for bounding box calculation)
        virtual float getHeight() const = 0; //override this (needed for bounding box calculation)
        virtual void drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const = 0; //override this when inheriting from node


        virtual void draw(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const final; //draws the node itself
        virtual void updateCurrent() {} //override this when inheriting from (by default does nothing at all
        virtual const sf::Transform getTotalTransform() const final; //gets the total inherited transformation for this node (the combined transformation of the parents chain)
        virtual const sf::FloatRect getTotalRect() const final; //gets the actual node bounding box taking the total transformation chain into account
        virtual const sf::FloatRect getUntransformedRect() const final { return sf::FloatRect(0, 0, getWidth(), getHeight()); } //helper function for getThisRect
        virtual const sf::FloatRect getThisRect() const final { return getTransform().transformRect(getUntransformedRect()); } //gets the actual node bounding box without taking the total transformation into account (it only considers the transformation of this node itself)

    public:
        //ctor
        Node();

        //getters
        bool isActive() const { return active; }
        sf::Time getUpdateRate() const { return updrate; }
        bool isOrphan() const { return (parent == nullptr); } //returns true if it is an orphan node (no parent)

        //setters
        void setActive(const bool isActive) { active = isActive; } //setting this on false will disable this node update function
        void setUpdateRate(const sf::Time theTime) { updrate = theTime; } //this node gets updated only everytime TIME seconds are passed, setting this to zero will update the node everytime the function is called

        //functions
        virtual sf::Vector2f getGlobalPosition() const final; //just returning the global bounding box top/left
        virtual sf::FloatRect getLocalBounds() const final { return getThisRect(); } //returns the node bounding box with only its transformation taken into account
        virtual sf::FloatRect getGlobalBounds() const final { return getTotalRect(); } //returns the node bounding box with all the transformation chain taken into account
        void attachChild(Node* const theChild); //attach a child to this node (if already attached to another node, it detaches it first)
        bool detachChild(Node* const theChild); //detach a child from this node (if already attached, does nothing otherwise)
        void detachAllChildren() { children.clear(); }
        virtual void update(const sf::Time theElapsedTime, const sf::FloatRect& theUpdRect = sf::FloatRect(0, 0, 0, 0)) final; //the node only gets updated if it is active, if enough time is passed or if its bounding box is inside UPDRECT (condition not checked if UPDRECT is 0, 0, 0, 0,)

        friend class Layer;
};



#endif // _NODE_HPP_
