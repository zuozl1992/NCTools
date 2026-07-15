cmake_minimum_required(VERSION 3.19)

if(NOT DEFINED CMAKE_PREFIX_PATH)
    message(STATUS "CMAKE_PREFIX_PATH not set. Please set it to your Qt 6.8.3 installation path.")
    message(STATUS "Example: cmake -DCMAKE_PREFIX_PATH=/path/to/qt6.8.3 ..")
endif()

project(NCTools LANGUAGES CXX VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Widgets
    SerialPort
    Network
    LinguistTools
    Test
)

# Qt version check
include(cmake/QtVersionCheck.cmake)

qt_standard_project_setup()

add_subdirectory(src)
add_subdirectory(tests)
