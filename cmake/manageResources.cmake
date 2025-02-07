function(download_file)
  set(oneValueArgs URL LOCATION)

  cmake_parse_arguments(DOWNLOAD "" "${oneValueArgs}" "" ${ARGN})

  if(NOT DEFINED DOWNLOAD_URL)
    message(
      FATAL_ERROR "Please add an url for the resource you want to download")
  endif()

  if(NOT DEFINED DOWNLOAD_LOCATION)
    message(
      FATAL_ERROR "Please add a download location where to save your file")
  endif()

  if(DEFINED DOWNLOAD_UNPARSED_ARGUMENTS)
    message(
      WARNING
        "Function called with unrecognized parameters: ${RESOURCE_UNPARSED_ARGUMENTS}"
    )
  endif()

  file(DOWNLOAD ${DOWNLOAD_URL} ${DOWNLOAD_LOCATION} STATUS DOWNLOAD_STATUS)

  list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
  if(NOT ${STATUS_CODE} EQUAL 0)
    message(FATAL_ERROR "Failed to download ${DOWNLOAD_URL}")
  endif()
endfunction()

function(add_resource)
  set(oneValueArgs IDENTIFIER FOLDER)

  cmake_parse_arguments(RESOURCE "" "${oneValueArgs}" "" ${ARGN})

  if(NOT DEFINED RESOURCE_IDENTIFIER)
    message(
      FATAL_ERROR
        "Please add an identifier for the resource you want to add (file local path or remote url)"
    )
  endif()

  if(NOT DEFINED RESOURCE_FOLDER)
    message(
      FATAL_ERROR
        "Please add a folder where to save your resource in the embedded filesystem"
    )
  endif()

  if(DEFINED RESOURCE_UNPARSED_ARGUMENTS)
    message(
      WARNING
        "Function called with unrecognized parameters: ${RESOURCE_UNPARSED_ARGUMENTS}"
    )
  endif()

  if(${RESOURCE_IDENTIFIER} MATCHES "^https://.*$" OR ${RESOURCE_IDENTIFIER}
                                                      MATCHES "^http://.*$")
    cmake_path(GET RESOURCE_IDENTIFIER FILENAME RESOURCE_FILENAME)
    set(RESOURCE_LOCATION
        "${CMAKE_BINARY_DIR}/resources/${RESOURCE_FOLDER}/${RESOURCE_FILENAME}")
    download_file(URL ${RESOURCE_IDENTIFIER} LOCATION ${RESOURCE_LOCATION})
  else()
    set(RESOURCE_LOCATION "${RESOURCE_IDENTIFIER}")
  endif()

  if(NOT EXISTS "${RESOURCE_LOCATION}")
    message(
      FATAL_ERROR
        "Resource ${RESOURCE_IDENTIFIER} does not exist and cannot be added")
  endif()

  cmake_path(GET RESOURCE_LOCATION PARENT_PATH RESOURCE_PARENT_PATH)

  cmrc_add_resources(${RESOURCE_LIBRARY} WHENCE ${RESOURCE_PARENT_PATH} PREFIX
                     ${RESOURCE_FOLDER} ${RESOURCE_LOCATION})
endfunction()

# Download CMakeRC
set(CMAKERC_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmakerc/CMakeRC.cmake")
set(CMAKERC_VERSION "2.0.1")
set(CMAKERC_URL
    "https://github.com/vector-of-bool/cmrc/releases/download/${CMAKERC_VERSION}/CMakeRC.cmake"
)
download_file(URL ${CMAKERC_URL} LOCATION ${CMAKERC_DOWNLOAD_LOCATION})
include(${CMAKERC_DOWNLOAD_LOCATION})

# Creating CMakeRC resource library
set(RESOURCE_LIBRARY "resources")
cmrc_add_resource_library(${RESOURCE_LIBRARY})

# Bundle resources
set(RESOURCE_URL "https://web.enea.geniorio.it/resources")

# Rom database
add_resource(IDENTIFIER "${CMAKE_SOURCE_DIR}/db/romdb.json" FOLDER "romdb")

# Input database
add_resource(IDENTIFIER "${CMAKE_SOURCE_DIR}/db/inputdb.json" FOLDER "inputdb")

# Font
add_resource(IDENTIFIER "${RESOURCE_URL}/fonts/inter.ttf" FOLDER "fonts")

# Audio
add_resource(IDENTIFIER "${RESOURCE_URL}/audio/launch.wav" FOLDER "audio")
add_resource(IDENTIFIER "${RESOURCE_URL}/audio/move.wav" FOLDER "audio")
