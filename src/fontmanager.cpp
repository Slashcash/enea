#include <utility>

#include "fontmanager.hpp"

const sf::Font& FontManager::getFont(const fs::path& theFontPath) {
    auto it = fontmap.find(theFontPath); //search for the font
    if( it != fontmap.end() ) return it->second; //if already loaded, return a reference to it
    else return loadFont(theFontPath); //load it otherwise
}

const sf::Font& FontManager::loadFont(const fs::path& theFontPath) {
    sf::Font buffer;
    buffer.loadFromFile(theFontPath); //effectively loads the font
    return fontmap.insert(std::make_pair(theFontPath, buffer)).first->second; //store it
}

void FontManager::loadFonts(const std::vector<fs::path>& thePaths) {
    for( auto it = thePaths.begin(); it < thePaths.end(); it++ )
        loadFont(*it);
}

