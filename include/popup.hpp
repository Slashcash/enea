#ifndef _POPUP_HPP_
#define _POPUP_HPP_

#include "rectangle.hpp"
#include "row.hpp"

/*this class represents a popup that will appear on screen to show some messages, add a properly constructed row
with the apposite function before drawing it for the right effect*/

class PopUp : public Rectangle {
    private:
        //members
        Row row; //the row with the text we will show in the popup
        float spacing; //the spacing between the popup and the row

        void calculateRowPosition(); //to be calculated when the row changes or when the spacing/rectangle dimension changes
        void calculateRowParameters(); //to be used when a row gets set, it will reset back some needed parameters if they are changed

    public:
        //ctor
        PopUp();

        //getters
        float getSpacing() const { return spacing; }

        //setters
        void setRow(const Row& theRow);
        void setSpacing(const float theSpacing) { spacing = theSpacing; calculateRowPosition(); }
        void setSize(const sf::Vector2f& theSize) { Rectangle::setSize(theSize); calculateRowPosition(); }
        void setActive(const bool isActive) { Rectangle::setActive(isActive); row.setActive(isActive); }
        void setUpdateRate(const sf::Time theTime) { Rectangle::setUpdateRate(theTime); row.setUpdateRate(theTime); }

};

#endif // _POPUP_HPP_
