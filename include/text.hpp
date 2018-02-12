#ifndef _TEXT_HPP_
#define _TEXT_HPP_

#include <SFML/Graphics/Text.hpp>

#include "node.hpp"

/*more or less a wrapper around SFML text*/

class Text : public Node {
    private:
        //members
        sf::Text text;

        //functions
        virtual void drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const;

    protected:
        virtual float getWidth() const { return text.getLocalBounds().width; }
        virtual float getHeight() const { return text.getLocalBounds().height; }

    public:
        //ctor
        Text() = default;
        Text(const sf::Font& theFont, const std::string& theString, unsigned int theSize = 30) { text.setFont(theFont); text.setString(theString); text.setCharacterSize(theSize); } //BE AWARE THAT ONLY THE REFERENCE TO THE FONT IS STORED, FONT LIFETIME HAS TO BE MANAGED EXTERNALLY

        //setters
        void setFont(const sf::Font& theFont) { text.setFont(theFont); } //BE AWARE THAT ONLY THE REFERENCE TO THE FONT IS STORED, FONT LIFETIME HAS TO BE MANAGED EXTERNALLY
        void setSize(const unsigned int theSize) { text.setCharacterSize(theSize); }
        void setString(const std::string& theString) { text.setString(theString); }
        void setColor(const sf::Color theColor) { text.setFillColor(theColor); }

        //getters
        std::string getString() const { return text.getString(); }
        sf::Color getColor() const { return text.getFillColor(); }
        const sf::Font* getFont() const { return text.getFont(); }
        unsigned int getSize() const { return text.getCharacterSize(); }
};

#endif // _TEXT_HPP_
