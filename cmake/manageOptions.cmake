set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(
  USE_POSIX_FILE_LIST
  "Use POSIX API for listing files in a directory as opposed to function provided by libstdc++ (only to be used on POSIX systems)"
  OFF)

if(USE_POSIX_FILE_LIST)
  add_definitions(-DUSE_POSIX_FILE_LIST)
endif()
