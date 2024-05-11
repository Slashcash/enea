add_executable(${EXECUTABLE} main.cpp)
target_link_libraries(${EXECUTABLE} PRIVATE ${EXECUTABLE}Lib ${EXECUTABLE}Gui)
