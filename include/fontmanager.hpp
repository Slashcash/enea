#ifndef _FONTMANAGER_HPP_
#define _FONTMANAGER_HPP_

#include <map>
#include <SFML/Graphics/Font.hpp>

#include "filesystem.hpp"

/*a manager for the font lifetime
TODO: migrating from std::string to fs::path, it is not using it yet becaust it is recycled from another library of mine*/

class FontManager {
    private:
        //members
        std::map<fs::path, const sf::Font> fontmap; //the map which contains the textures

        //functions
        const sf::Font& loadFont(const fs::path& theFontPath);

    public:
        //functions
        const sf::Font& getFont(const fs::path& theFontPath); //BE AWARE THIS RETURNS A REFERENCE
        void loadFonts(const std::vector<fs::path>& thePaths); //this function loads a large number of textures in one hit (BE AWARE THIS CAN BE HEAVY)
};



#endif // _FONTMANAGER_HPP_
