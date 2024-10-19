find_package(GTest 1.14.0 REQUIRED)
enable_testing()

add_executable(
  ${EXECUTABLE}Test
  mock/romdb_mock.hpp
  source/romdb_test.cpp
  mock/configuration_mock.hpp
  source/configuration_test.cpp
  mock/romsource_mock.hpp
  source/romsource_test.cpp
  mock/systemcommand_mock.hpp
  source/systemcommand_test.cpp
  source/rom_test.cpp
  mock/inputmanager_mock.hpp
  # source/inputmanager_test.cpp
  # TODO: Re-enable once we figure out how to run in a docker without X11
  mock/node_mock.hpp
  # source/node_test.cpp
  # TODO: Re-enable once we figure out how to run in a docker without X11
  mock/resourcemanager_mock.hpp
  source/resourcemanager_test.cpp
  source/rominfo_test.cpp
  source/rommedia_test.cpp
  source/utils_test.cpp)

target_link_libraries(
  ${EXECUTABLE}Test PRIVATE GTest::gtest_main GTest::gmock ${EXECUTABLE}Lib
                            ${EXECUTABLE}Gui)

target_include_directories(${EXECUTABLE}Test PRIVATE mock)
include(GoogleTest)
gtest_discover_tests(${EXECUTABLE}Test)
