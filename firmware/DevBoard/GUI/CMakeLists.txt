cmake_minimum_required(VERSION 3.10)

project(DarknetGUI)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(../darknet-7/Src/darknet lib/darknet)

add_subdirectory(./3rdparty/wxWidgets lib/wxWidgets)

include_directories(include)

# set(wxWidgets_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/wxWidgets)
# set(wxWidgets_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/wxWidgets/lib/vc_x64_dll)
# set(wxWidgets_CONFIGURATION msw)
# set(wxBUILD_SHARED FALSE) #change this to TRUE to build wxWidgets as a shared library 

#find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
#include(${wxWidgets_USE_FILE})



add_executable(DarknetGUI WIN32 Src/GuiMain.cpp)
#target_link_libraries(DarknetGUI ${wxWidgets_LIBRARIES})
target_link_libraries(DarknetGUI wx::net wx::core wx::base)


if(WIN32)
    # Include a RC file for windows
    list(APPEND SRC_FILES ../sample.rc)
endif()

#find_package(wxWidgets REQUIRED core base net)