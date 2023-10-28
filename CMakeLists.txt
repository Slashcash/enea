# Setting minimum cmake required version
cmake_minimum_required(VERSION 3.27.0)

include(cmake/manageOptions.cmake)

# Setting up project
project(
  ${SOFTWARE_NAME}
  VERSION ${SOFTWARE_VERSION}
  DESCRIPTION ${SOFTWARE_DESCRIPTION}
  HOMEPAGE_URL ${SOFTWARE_URL}
  LANGUAGES CXX)

include(cmake/manageDependencies.cmake)
include(cmake/manageResources.cmake)

# Creating executable and library (mainly used to ease testing)
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
  include/resourcemanager.hpp)
target_include_directories(${EXECUTABLE}Lib PUBLIC include)
target_link_libraries(
  ${EXECUTABLE}Lib
  PUBLIC spdlog::spdlog
         magic_enum::magic_enum
         sfml-system
         sfml-window
         sfml-graphics
         rocket::rocket
         nlohmann_json::nlohmann_json
         ${RESOURCE_LIBRARY})

add_executable(${EXECUTABLE} main.cpp)
target_link_libraries(${EXECUTABLE} PRIVATE ${EXECUTABLE}Lib)

# Generating files
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file("version.hpp.in" "${GENERATED_DIR}/version.hpp")
target_include_directories(${EXECUTABLE}Lib PUBLIC ${GENERATED_DIR})

# Managing tests (separately)
if(${OPT_PREFIX}_BUILD_TESTS)
  add_subdirectory(test)
endif()
