cmake_minimum_required(VERSION 3.10)

project(DarknetGUI)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(../darknet-7/Src/darknet lib/darknet)

add_subdirectory(lib/wxWidgets)
include_directories(include)
add_executable(DarknetGUI WIN32 Src/GuiMain.cpp)
target_link_libraries(DarknetGUI darknet7 wx::net wx::core wx::base)