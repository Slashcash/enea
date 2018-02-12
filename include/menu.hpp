#ifndef _MENU_HPP_
#define _MENU_HPP_

#include "rectangle.hpp"
#include "list.hpp"

class Menu : public Rectangle {
    private:
        //members
        List list;
        float spacing;

        //functions
        void calculateListPosition();
        void calculateListParameters();

    public:
        //ctor
        Menu();

        //getters
        float getSpacing() const { return spacing; }
        unsigned int getSelected() const { return list.getSelected(); }

        //setters
        void setList(const List& theList);
        void setSpacing(const float theValue) { spacing = theValue; calculateListPosition(); }
        void setSize(const sf::Vector2f& theSize) { Rectangle::setSize(theSize); calculateListPosition(); }
        void setActive(const bool isActive) { Rectangle::setActive(isActive); list.setActive(isActive); }
        void setUpdateRate(const sf::Time theTime) { Rectangle::setUpdateRate(theTime); list.setUpdateRate(theTime); }
        void setSelected(const unsigned int theValue) { list.setSelected(theValue); }

};

#endif // _MENU_HPP_
