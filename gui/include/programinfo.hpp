#ifndef PROGRAMINFO_HPP
#define PROGRAMINFO_HPP

#include <string_view>

#include <SFML/Graphics/RenderStates.hpp>

#include "node.hpp"

namespace sf {
class RenderTarget;
}

class ProgramInfo : public Node
{
 private:
    static constexpr std::string_view FONT_PATH = "fonts/inter.ttf";
    static constexpr unsigned int FONT_SIZE = 16;
    static constexpr float SPACING = 5.0F;

    void inline drawEffective(sf::RenderTarget& target, sf::RenderStates states) const override
    {
    }

 public:
    ProgramInfo();
};

#endif // PROGRAMINFO_HPP
