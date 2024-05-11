# Setting minimum cmake required version
cmake_minimum_required(VERSION 3.27.0)

# Setting up project
include(cmake/parseVariables.cmake)
project(
  ${SOFTWARE_NAME}
  VERSION ${SOFTWARE_VERSION}
  LANGUAGES CXX)

# Using C++20
set(CMAKE_CXX_STANDARD 20)

# Managing project options
include(cmake/manageOptions.cmake)

# Managing project dependencies
include(cmake/manageDependencies.cmake)

# Managing project assets
include(cmake/manageResources.cmake)

# Creating library
add_subdirectory(lib)

# Creating gui
add_subdirectory(gui)

# Creating executable
add_subdirectory(app)

# Creating tests
if(BUILD_TESTS)
  add_subdirectory(test)
endif()

# Installing
install(TARGETS ${EXECUTABLE} DESTINATION bin)
