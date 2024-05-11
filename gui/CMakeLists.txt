add_library(
  ${EXECUTABLE}Gui
  include/gui.hpp
  source/gui.cpp
  include/inputmanager.hpp
  source/inputmanager.cpp
  include/resourcemanager.hpp
  include/internalresourcemanager.hpp
  include/externalresourcemanager.hpp
  include/node.hpp
  source/node.cpp
  include/programinfo.hpp
  source/programinfo.cpp
  include/rommenu.hpp
  source/rommenu.cpp)

target_include_directories(${EXECUTABLE}Gui PUBLIC include)

target_link_libraries(
  ${EXECUTABLE}Gui
  PUBLIC sfml-window
  PRIVATE ${RESOURCE_LIBRARY} ${EXECUTABLE}Lib sfml-system sfml-graphics
          sfml-audio)
