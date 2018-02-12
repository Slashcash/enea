#ifndef _ROW_HPP_
#define _ROW_HPP_

#include "text.hpp"

class Row : public Text {
    private:
        //members
        std::size_t start_position; //variables used when the string is truncated
        std::size_t truncation_size;
        float truncate_width; //setting this to a value != 0 will truncate the row to this width

        //functions
        virtual void drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const;
        virtual void updateCurrent();
        void calculateTruncation(); //this function should be called every time
        virtual float getWidth() const { if( truncate_width > 0 ) return std::min(truncate_width, Text::getWidth()); else return Text::getWidth(); }


    public:
        //ctor
        Row();
        Row(const sf::Font& theFont, const std::string& theText);

        //getters
        float getTruncateWidth() const { return truncate_width; }

        //setters
        void setTruncateWidth(const float theWidth) { truncate_width = theWidth; calculateTruncation(); }
        void setFont(const sf::Font& theFont) { Text::setFont(theFont); calculateTruncation(); } //BE AWARE THAT ONLY THE REFERENCE TO THE FONT IS STORED, FONT LIFETIME HAS TO BE MANAGED EXTERNALLY
        void setSize(const unsigned int theSize) { Text::setSize(theSize); calculateTruncation(); }
        void setString(const std::string& theString) { Text::setString(theString); }
        void setActive(const bool isActive) { start_position = 0; Node::setActive(isActive); }
};

#endif
