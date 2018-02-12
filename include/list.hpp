#ifndef _LIST_HPP_
#define _LIST_HPP_

#include <vector>

#include "node.hpp"
#include "row.hpp"

/*This class represents a list of row displayed in order, it can be set to a fixed height and, if necessary it gets divided in different pages*/

class List : public Node {
    public:
        //enum
        enum Style {
            Normal,
            Centered
        };

    private:
        std::vector<Row> rows; //all the rows are stored here
        unsigned int page; //the page we're currently at
        float spacing; //the spacing between two rows
        float truncate_height;
        float truncate_width;
        sf::Color main_color; //the main color for the row of this list
        sf::Color highlight_color;
        std::vector<std::vector<Row*>> pages; //all the different pages
        unsigned int selected; //the selected element in the list
        Style style;

        virtual void drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const;
        void calculatePages(); //this function recalculates the pages number everytime something changes
        void setAllParameters(); //used when the list is filled with a new vector and it has to be refed again with correct parameters
        virtual float getWidth() const;
        virtual float getHeight() const;
        void setPage(const unsigned int thePage); //this function is used when in need to change the page of this list

    public:
        //ctor
        List();
        List(const std::vector<Row>& theRows); //use this ctor if you want a ready-to-display list

        //setters
        void setRowVector(const std::vector<Row>& theRows);
        void setMainColor(const sf::Color& theColor) { main_color = theColor; for(auto it = rows.begin(); it < rows.end(); it++) it->setColor(main_color); }
        void setTruncateWidth(const float theTruncateWidth) { truncate_width = theTruncateWidth; for(auto it = rows.begin(); it < rows.end(); it++) it->setTruncateWidth(truncate_width); }
        void setTruncateHeight(const float theTruncateHeight) { truncate_height = theTruncateHeight; calculatePages(); }
        void setSpacing(const float theSpacing) { spacing = theSpacing; calculatePages(); }
        void setUpdateRate(const sf::Time theTime) { for(auto it = rows.begin(); it < rows.end(); it++) it->setUpdateRate(theTime); Node::setUpdateRate(theTime); }
        void setSelected(const unsigned int theSelection);
        void setHighLightColor(const sf::Color& theColor) { highlight_color = theColor; if( !rows.empty() && isActive() ) rows[selected].setColor(highlight_color); }
        void setStyle(const Style& theStyle) { style = theStyle; calculatePages(); }

        //getters
        float getSpacing() const { return spacing; }
        float getTruncateHeight() const { return truncate_height; }
        float getTruncateWidth() const { return truncate_width; }
        sf::Color getMainColor() const { return main_color; }
        sf::Color getHighLightCoor() const { return highlight_color; }
        unsigned int getSelected() const { return selected; }
        void setActive(const bool isActive);
        Style getStyle() const { return style; }
        unsigned int getElementNumber() const { return rows.size(); }
};

#endif // _LIST_HPP_
