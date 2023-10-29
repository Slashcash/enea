find_package(GTest 1.14.0 REQUIRED)

enable_testing()

add_executable(
  ${EXECUTABLE}Test
  source/rom_test.cpp
  mock/rom_mock.hpp
  source/romsource_test.cpp
  mock/romsource_mock.hpp
  source/configuration_test.cpp
  mock/configuration_mock.hpp
  source/inputmanager_test.cpp
  mock/inputmanager_mock.hpp
  source/rommenu_test.cpp)

target_link_libraries(${EXECUTABLE}Test GTest::gtest_main GTest::gmock
                      ${EXECUTABLE}Lib ${EXECUTABLE}Gui)
target_include_directories(${EXECUTABLE}Test PRIVATE mock)
include(GoogleTest)
gtest_discover_tests(${EXECUTABLE}Test)
