#include "row.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

Row::Row() : Text() {
    start_position = 0;
    truncation_size = std::string::npos;
    truncate_width = 0;
}

Row::Row(const sf::Font& theFont, const std::string& theText) : Text(theFont, theText) {
    start_position = 0;
    truncation_size = std::string::npos;
    truncate_width = 0;
}

void Row::calculateTruncation() {
    if( truncate_width < 0 ) truncate_width = 0; //if someone has stupidly set the truncate_width < 0 we correct it

    if( truncate_width == 0 ) { //if the truncation is 0 we go back to a full string
        start_position = 0;
        truncation_size = std::string::npos;
    }

    else {
        std::size_t buffer_pos = getString().size();
        std::string buffer_str = getString();
        Text buffer;
        buffer.setFont(*getFont());
        buffer.setColor(getColor());
        buffer.setSize(getSize());
        buffer.setString(buffer_str);

        //this algorithm operates on a buffer text and discovers how many character we should include to fit the truncate width
        while( buffer.getLocalBounds().width > truncate_width && buffer_pos > start_position ) {
            buffer_pos--;
            buffer.setString(buffer_str.substr(0, buffer_pos)+"...");
        }

        truncation_size = buffer_pos;

    }

}

void Row::drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const {
    //building the text we want to draw (we will not draw this directly in case it does need truncation
    Text buffer;
    buffer.setFont(*getFont());
    buffer.setColor(getColor());
    buffer.setSize(getSize());
    if( truncation_size == std::string::npos || truncation_size == getString().size() || start_position + truncation_size == getString().size() ) buffer.setString(getString().substr(start_position, truncation_size));
    else buffer.setString(getString().substr(start_position, truncation_size)+"...");

    theRenderTarget.draw(buffer, theRenderStates);
}

void Row::updateCurrent() {
    if( truncation_size != std::string::npos && truncation_size != getString().size() ) {
        start_position++;


        if( start_position + truncation_size == getString().size()+1) {
            start_position = 0;

        }
    }
}
