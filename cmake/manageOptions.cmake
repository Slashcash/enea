set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# USE_POSIX_FILE_LIST
option(
  USE_POSIX_FILE_LIST
  "Use POSIX API for listing files in a directory as opposed to function provided by libstdc++ (only to be used on POSIX systems)"
  OFF)

if(USE_POSIX_FILE_LIST)
  # Only meaningful on posix systems, if set on non-posix we abort
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DUSE_POSIX_FILE_LIST)
  else()
    message(
      FATAL_ERROR
        "USE_POSIX_FILE_LIST cannot be activated when targeting non posix system"
    )
  endif()
endif()

# USE_DIRECT_RENDERING
option(
  USE_DIRECT_RENDERING
  "Use direct rendering when drawing on the screen. This mode can be used on machines without a graphical interface"
  OFF)

if(USE_DIRECT_RENDERING)
  # Only meaningful on linux systems, if set on non-linux we abort
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DUSE_DIRECT_RENDERING)
  else()
    message(
      FATAL_ERROR
        "USE_DIRECT_RENDERING cannot be activated on non-Linux systems")
  endif()
endif()

# Printing out an option summary
message(
  "
-----OPTION SUMMARY-----
USE_POSIX_FILE_LIST: ${USE_POSIX_FILE_LIST}
USE_DIRECT_RENDERING: ${USE_DIRECT_RENDERING}
------------------------
")
