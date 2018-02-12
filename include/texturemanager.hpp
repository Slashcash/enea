#ifndef _TEXTUREMANAGER_HPP_
#define _TEXTUREMANAGER_HPP_

#include <map>
#include <SFML/Graphics/Texture.hpp>

#include "filesystem.hpp"

/*a manager for the texture lifetime*/
class TextureManager {
    private:
        //members
        std::map<const fs::path, const sf::Texture> texmap; //the map which contains the textures

        //functions
        const sf::Texture& loadTexture(const fs::path& theTexturePath);

    public:
        //functions
        const sf::Texture& getTexture(const fs::path& theTexturePath); //BE AWARE THIS RETURNS A REFERENCE
        void loadTextures(const std::vector<fs::path>& thePaths); //this function loads a large number of textures in one hit (BE AWARE THIS CAN BE HEAVY)
};



#endif // _TEXTUREMANAGER_HPP_
