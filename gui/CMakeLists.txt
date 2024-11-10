add_library(
  ${EXECUTABLE}Gui
  include/gui.hpp
  source/gui.cpp
  include/resourcemanager.hpp
  include/internalresourcemanager.hpp
  include/externalresourcemanager.hpp
  include/node.hpp
  source/node.cpp
  include/programinfo.hpp
  source/programinfo.cpp
  include/rommenu.hpp
  source/rommenu.cpp
  # Input
  include/input/device.hpp
  source/input/device.cpp
  include/input/mapping.hpp
  source/input/mapping.cpp
  include/input/button.hpp
  source/input/button.cpp
  include/input/manager.hpp
  source/input/manager.cpp
  include/input/identification.hpp)

target_include_directories(${EXECUTABLE}Gui PUBLIC include)

target_link_libraries(
  ${EXECUTABLE}Gui
  PUBLIC sfml-window
  PRIVATE ${RESOURCE_LIBRARY} ${EXECUTABLE}Lib sfml-system sfml-graphics
          sfml-audio)
