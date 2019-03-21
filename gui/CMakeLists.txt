cmake_minimum_required(VERSION 3.12)

project(gui)

set(CMAKE_CXX_STANDARD 17)

# Find includes in corresponding build directories
# CHANGE PATH!!!!
set(CMAKE_PREFIX_PATH "/home/fj/Qt/5.12.2/gcc_64/")
set(CMAKE_AUTOMOC  ON)
set(CMAKE_AUTOUIC ON)
find_package( Qt5Core REQUIRED)
find_package( Qt5Widgets REQUIRED)

set(SOURCE_FILES main.cpp)

add_executable(gui ${SOURCE_FILES})
target_link_libraries(gui Qt5::Widgets Qt5::Core )