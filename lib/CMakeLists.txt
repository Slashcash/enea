add_library(
  ${EXECUTABLE}Lib
  include/database.hpp
  include/rom/game.hpp
  source/rom/game.cpp
  include/configuration.hpp
  source/configuration.cpp
  include/exception.hpp
  include/systemcommand.hpp
  source/systemcommand.cpp
  include/emulator.hpp
  source/emulator.cpp
  include/rom/info.hpp
  include/rom/media.hpp
  include/rom/source.hpp
  source/rom/source.cpp
  include/rom/folder.hpp
  source/rom/folder.cpp
  include/utils.hpp
  include/singleton.hpp
  include/model.hpp)

target_include_directories(${EXECUTABLE}Lib PUBLIC include)
target_link_libraries(
  ${EXECUTABLE}Lib
  PUBLIC spdlog::spdlog
         nlohmann_json::nlohmann_json
         rocket::rocket
         magic_enum::magic_enum
         stduuid::stduuid
         chef-fun::chef-fun
         system2.cpp::system2.cpp
         ${RESOURCE_LIBRARY})

# Generating files
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file("include/softwareinfo.hpp.in"
               "${GENERATED_DIR}/softwareinfo.hpp")
target_include_directories(${EXECUTABLE}Lib PUBLIC ${GENERATED_DIR})
