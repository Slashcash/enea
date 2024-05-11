set(CMAKERC_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmakerc/CMakeRC.cmake")
set(CMAKERC_VERSION "2.0.1")
set(CMAKERC_URL
    "https://github.com/vector-of-bool/cmrc/releases/download/${CMAKERC_VERSION}/CMakeRC.cmake"
)

set(RESOURCE_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/resources")
set(RESOURCE_URL "enea.geniorio.it/${SOFTWARE_VERSION}")

set(RESOURCE_LIBRARY "resources")

file(DOWNLOAD ${CMAKERC_URL} ${CMAKERC_DOWNLOAD_LOCATION})
include(${CMAKERC_DOWNLOAD_LOCATION})

cmrc_add_resource_library(${RESOURCE_LIBRARY})

function(add_resource)
  set(oneValueArgs NAME FOLDER)

  cmake_parse_arguments(RESOURCE "" "${oneValueArgs}" "" ${ARGN})

  if(NOT DEFINED RESOURCE_NAME)
    message(
      FATAL_ERROR "Please add a name for the resource you want to retrieve")
  endif()

  if(NOT DEFINED RESOURCE_FOLDER)
    message(FATAL_ERROR "Please add a folder where to save your resource in")
  endif()

  if(DEFINED RESOURCE_UNPARSED_ARGUMENTS)
    message(
      WARNING
        "Function called with unrecognized parameters: ${RESOURCE_UNPARSED_ARGUMENTS}"
    )
  endif()

  file(DOWNLOAD "${RESOURCE_URL}/${RESOURCE_NAME}"
       "${RESOURCE_DOWNLOAD_LOCATION}/${RESOURCE_NAME}")

  cmake_path(GET RESOURCE_NAME PARENT_PATH PARENT)

  cmrc_add_resources(
    ${RESOURCE_LIBRARY} WHENCE ${RESOURCE_DOWNLOAD_LOCATION}/${PARENT} PREFIX
    ${RESOURCE_FOLDER} ${RESOURCE_DOWNLOAD_LOCATION}/${RESOURCE_NAME})
endfunction()

# Download resources

# Rom database
add_resource(NAME "romdb/romdb.xml" FOLDER "romdb")

# Font
add_resource(NAME "fonts/inter.ttf" FOLDER "fonts")

# Audio
add_resource(NAME "audio/launch.wav" FOLDER "audio")
add_resource(NAME "audio/move.wav" FOLDER "audio")
