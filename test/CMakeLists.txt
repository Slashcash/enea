find_package(GTest 1.14.0 REQUIRED)
enable_testing()

add_executable(
  ${EXECUTABLE}Test
  source/main.cpp
  mock/database/table_mock.hpp
  source/database/table_test.cpp
  mock/utils/lazy_mock.hpp
  source/utils/lazy_test.cpp
  mock/configuration_mock.hpp
  source/configuration_test.cpp
  mock/romsource_mock.hpp
  source/romsource_test.cpp
  mock/systemcommand_mock.hpp
  source/systemcommand_test.cpp
  source/romgame_test.cpp
  mock/emulator_mock.hpp
  source/emulator_test.cpp
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
  source/utils_test.cpp
  source/inputbutton_test.cpp
  source/inputmapping_test.cpp
  source/inputidentification_test.cpp
  source/model_test.cpp)

target_link_libraries(
  ${EXECUTABLE}Test PRIVATE GTest::gtest GTest::gmock ${EXECUTABLE}Lib
                            ${EXECUTABLE}Gui)

target_include_directories(${EXECUTABLE}Test PRIVATE mock)
include(GoogleTest)
gtest_discover_tests(${EXECUTABLE}Test)
