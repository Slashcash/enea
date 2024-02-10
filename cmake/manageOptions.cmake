set(ENV_FILEPATH "${CMAKE_CURRENT_SOURCE_DIR}/.env")

function(parse_environment)
  set(oneValueArgs NAME FILEPATH)

  cmake_parse_arguments(ENV "" "${oneValueArgs}" "" ${ARGN})

  set(dotenv ${ENV_FILEPATH})

  if(NOT EXISTS "${dotenv}")
    message(FATAL_ERROR "Dot-env file not found: ${dotenv}")
  endif()

  file(STRINGS "${dotenv}" entries)
  foreach(entry IN LISTS entries)
    if(entry MATCHES "^([^=]+)=(.*)$")
      set(ENV{${CMAKE_MATCH_1}} "${CMAKE_MATCH_2}")
    else()
      message(FATAL_ERROR "Malformed dotenv entry:\n${entry}")
    endif()
  endforeach()

endfunction()

# Setting C++20 as default
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Loading environment file
parse_environment(FILEPATH ${ENV_FILEPATH})

# Setting and validating project name
set(SOFTWARE_NAME "$ENV{SOFTWARE_NAME}")

if(NOT ${SOFTWARE_NAME} MATCHES "^[A-Za-z]+$")
  message(
    FATAL_ERROR "SOFTWARE_NAME variable does not contain a valid software name."
  )
endif()
string(TOUPPER ${SOFTWARE_NAME} OPT_PREFIX)
string(TOLOWER ${SOFTWARE_NAME} EXECUTABLE)

# Setting and validating project version
set(SOFTWARE_VERSION "$ENV{SOFTWARE_VERSION}")

if(NOT ${VERSION} MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
  message(
    FATAL_ERROR
      "SOFTWARE_VERSION variable does not contain a valid version number.")
endif()

# Setting and validating project description
set(SOFTWARE_DESCRIPTION "$ENV{SOFTWARE_DESCRIPTION}")

if(NOT SOFTWARE_DESCRIPTION)
  message(
    FATAL_ERROR
      "SOFTWARE_DESCRIPTION variable does not contain a valid description.")
endif()

# Setting and validating project url
set(SOFTWARE_URL "$ENV{SOFTWARE_URL}")

if(NOT SOFTWARE_URL)
  message(FATAL_ERROR "SOFTWARE_URL variable does not contain a valid url.")
endif()

# Managing project options
option(${OPT_PREFIX}_BUILD_TESTS "Build project tests" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
