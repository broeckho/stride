cmake_minimum_required(VERSION 3.12)

project(gui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC  ON)
set(CMAKE_AUTOUIC ON)

find_package( Qt5 COMPONENTS Widgets Core Qml REQUIRED)

set(SOURCE_FILES main.cpp)

add_executable(gui ${SOURCE_FILES})
target_link_libraries(gui Qt5::Widgets Qt5::Core Qt5::Qml)
