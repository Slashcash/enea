# --- helper function that parses environment file
function(parse_environment)
  set(oneValueArgs NAME FILEPATH)

  cmake_parse_arguments(ENV "" "${oneValueArgs}" "" ${ARGN})

  set(dotenv ${ENV_FILEPATH})

  if(EXISTS "${dotenv}")
    file(STRINGS "${dotenv}" entries)
    foreach(entry IN LISTS entries)
      if(entry MATCHES "^([^=]+)=(.*)$")
        set(ENV{${CMAKE_MATCH_1}} "${CMAKE_MATCH_2}")
      else()
        message(FATAL_ERROR "Malformed dotenv entry:\n${entry}")
      endif()
    endforeach()
  endif()

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
find_package(Git)
if(Git_FOUND)
  # Get if head is currently on a tag
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --exact-match
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE GIT_TAG_ON_HEAD)

  if(NOT GIT_TAG_ON_HEAD)
    # Getting the current git tag
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE GIT_TAG)

    # Getting the current git commit
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE GIT_COMMIT)

    # Construct the final version string with commit
    set(SOFTWARE_VERSION "${GIT_TAG}+${GIT_COMMIT}")
    string(SUBSTRING "${GIT_TAG}" 1 -1 SOFTWARE_TAG)
  else()
    # Construct the final version with tag only
    set(SOFTWARE_VERSION "${GIT_TAG_ON_HEAD}")
    string(SUBSTRING "${GIT_TAG_ON_HEAD}" 1 -1 SOFTWARE_TAG)
  endif()
else()
  message(FATAL_ERROR "Git executable not found")
endif()

if(NOT ${SOFTWARE_TAG} MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
  message(
    FATAL_ERROR "SOFTWARE_TAG variable does not contain a valid version number."
  )
endif()
