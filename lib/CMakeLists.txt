add_library(
  ${EXECUTABLE}Lib
  include/romdb.hpp
  source/romdb.cpp
  include/rom.hpp
  source/rom.cpp
  include/configuration.hpp
  source/configuration.cpp
  include/exception.hpp
  include/romsource.hpp
  source/romsource.cpp
  include/systemcommand.hpp
  source/systemcommand.cpp
  include/emulator.hpp
  source/emulator.cpp
  include/cache.hpp
  source/cache.cpp
  include/rominfo.hpp
  source/rominfo.cpp
  include/rommedia.hpp
  source/rommedia.cpp)

target_include_directories(${EXECUTABLE}Lib PUBLIC include)
target_link_libraries(
  ${EXECUTABLE}Lib
  PUBLIC spdlog::spdlog nlohmann_json::nlohmann_json rocket::rocket
         magic_enum::magic_enum ${RESOURCE_LIBRARY})

# Generating files
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file("include/softwareinfo.hpp.in"
               "${GENERATED_DIR}/softwareinfo.hpp")
target_include_directories(${EXECUTABLE}Lib PUBLIC ${GENERATED_DIR})
