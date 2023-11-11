# Setting C++20 as default
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Setting and validating project name
set(SOFTWARE_NAME "Enea")

if(NOT ${SOFTWARE_NAME} MATCHES "^[A-Za-z]+$")
  message(
    FATAL_ERROR "SOFTWARE_NAME variable does not contain a valid software name."
  )
endif()
string(TOUPPER ${SOFTWARE_NAME} OPT_PREFIX)
string(TOLOWER ${SOFTWARE_NAME} EXECUTABLE)

# Setting and validating project version
set(SOFTWARE_VERSION "0.2.0")

if(NOT ${VERSION} MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
  message(
    FATAL_ERROR
      "SOFTWARE_VERSION variable does not contain a valid version number.")
endif()

# Setting and validating project description
set(SOFTWARE_DESCRIPTION "Graphical arcade game frontend")

if(NOT SOFTWARE_DESCRIPTION)
  message(
    FATAL_ERROR
      "SOFTWARE_DESCRIPTION variable does not contain a valid description.")
endif()

# Setting and validating project url
set(SOFTWARE_URL "https://github.com/Slashcash/enea")

if(NOT SOFTWARE_URL)
  message(FATAL_ERROR "SOFTWARE_URL variable does not contain a valid url.")
endif()

# Managing project options
option(${OPT_PREFIX}_BUILD_TESTS "Build project tests" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
