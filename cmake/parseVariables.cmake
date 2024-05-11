# --- helper function that parses environment file
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
# ---

# Loading environment file to parse shared variables
parse_environment(FILEPATH "${CMAKE_CURRENT_SOURCE_DIR}/.env")

# Setting and validating project name
set(SOFTWARE_NAME "$ENV{SOFTWARE_NAME}")

if(NOT ${SOFTWARE_NAME} MATCHES "^[A-Za-z]+$")
  message(
    FATAL_ERROR "SOFTWARE_NAME variable does not contain a valid software name."
  )
endif()
string(TOLOWER ${SOFTWARE_NAME} EXECUTABLE)

# Setting and validating project version
set(SOFTWARE_VERSION "$ENV{SOFTWARE_VERSION}")

if(NOT ${VERSION} MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
  message(
    FATAL_ERROR
      "SOFTWARE_VERSION variable does not contain a valid version number.")
endif()
