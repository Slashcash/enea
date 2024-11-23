#ifndef GUI_HPP
#define GUI_HPP

#include <list>

#include "rom/game.hpp"

class Gui
{
 private:
    static constexpr unsigned int SCENE_WIDTH = 1920;
    static constexpr unsigned int SCENE_HEIGHT = 1080;
    static constexpr unsigned int MAX_FRAME_RATE = 30;

    std::vector<Rom::Game> mRomList;

 public:
    Gui() = delete;
    explicit Gui(const std::vector<Rom::Game>& romList);

    void run();
};

#endif // GUI_HPP
