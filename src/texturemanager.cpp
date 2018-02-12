#include <utility>

#include "texturemanager.hpp"

const sf::Texture& TextureManager::getTexture(const fs::path& theTexturePath) {
    auto it = texmap.find(theTexturePath); //search for the textures
    if( it != texmap.end() ) return it->second; //if already loaded, return a reference to it
    else return loadTexture(theTexturePath); //load it otherwise
}

const sf::Texture& TextureManager::loadTexture(const fs::path& theTexturePath) {
    sf::Texture buffer;
    buffer.loadFromFile(theTexturePath); //effectively load the texture
    return texmap.insert(std::make_pair(theTexturePath, buffer)).first->second; //store it
}

void TextureManager::loadTextures(const std::vector<fs::path>& thePaths) {
    for( auto it = thePaths.begin(); it < thePaths.end(); it++ )
        loadTexture(*it);
}
