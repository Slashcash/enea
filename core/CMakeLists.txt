add_library(
  ${EXECUTABLE}Lib
  source/rom.cpp
  include/rom.hpp
  source/romsource.cpp
  include/romsource.hpp
  source/configuration.cpp
  include/configuration.hpp
  source/inputmanager.cpp
  include/inputmanager.hpp
  source/romdb.cpp
  include/romdb.hpp
  source/systemcommand.cpp
  include/systemcommand.hpp
  source/emulator.cpp
  include/emulator.hpp
  include/resourcemanager.hpp)

target_include_directories(${EXECUTABLE}Lib PUBLIC include)

target_link_libraries(
  ${EXECUTABLE}Lib
  PUBLIC spdlog::spdlog
         magic_enum::magic_enum
         sfml-system
         sfml-window
         sfml-graphics
         sfml-audio
         rocket::rocket
         nlohmann_json::nlohmann_json
         tinyxml2::tinyxml2
         ${RESOURCE_LIBRARY})

# Generating files
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file("version.hpp.in" "${GENERATED_DIR}/version.hpp")
target_include_directories(${EXECUTABLE}Lib PUBLIC ${GENERATED_DIR})
