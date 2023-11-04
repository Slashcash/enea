add_library(${EXECUTABLE}Gui source/rommenu.cpp include/rommenu.hpp
                             source/rominfo.cpp include/rominfo.hpp)

target_include_directories(${EXECUTABLE}Gui PUBLIC include)

target_link_libraries(${EXECUTABLE}Gui PUBLIC ${EXECUTABLE}Lib)
