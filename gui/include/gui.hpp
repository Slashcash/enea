#ifndef GUI_HPP
#define GUI_HPP

#include <list>

#include "rom.hpp"

class Gui
{
 private:
    static constexpr unsigned int SCENE_WIDTH = 1920;
    static constexpr unsigned int SCENE_HEIGHT = 1080;

    std::list<Rom> mRomList;

 public:
    Gui() = delete;
    explicit Gui(const std::list<Rom>& romList);

    void run();
};

#endif // GUI_HPP
