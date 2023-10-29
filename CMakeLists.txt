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

add_subdirectory(core)
add_subdirectory(gui)

add_executable(${EXECUTABLE} main.cpp)
target_link_libraries(${EXECUTABLE} PRIVATE ${EXECUTABLE}Lib ${EXECUTABLE}Gui)

# Managing tests (separately)
if(${OPT_PREFIX}_BUILD_TESTS)
  add_subdirectory(test)
endif()
