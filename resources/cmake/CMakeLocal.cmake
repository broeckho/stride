#############################################################################
#  This file is part of the Stride software.
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or any
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2017, Willem L, Kuylen E & Broeckhove J
#############################################################################
#
#        Local (at your site or for you personally) configuration.
#        We adopted this approach because IDE's do not
#        allways pick up your personalized environment variables.
#
#############################################################################

message(STATUS "\nReading configuration info from CMakeLocal.cmake! \n")

#============================================================================
# MACRO (WARNING: CLion overrides the setting of CMAKE_BUILD_TYPE)
#============================================================================
set(STRIDE_INCLUDE_DOC      FALSE)
set(STRIDE_FORCE_NO_OPENMP  FALSE)
set(STRIDE_FORCE_NO_PYTHON  FALSE)
set(STRIDE_COMPILER_ID      GNU)
#set(STRIDE_COMPILER_ID      Clang)
#set(STRIDE_COMPILER_ID      Apple)

#set(CMAKE_CXX_FLAGS “—Weffc++ Wextra -pedantic")
#set(CMAKE_CXX_FLAGS "-UNDEBUG")

#set(CMAKE_BUILD_TYPE "Debug")

#============================================================================
# To help find modules.
#============================================================================
if(APPLE)
    set(CMAKE_PREFIX_PATH "/opt/local/Library/Frameworks/Python.framework/Versions/3.6;$ENV{PATH}")
endif()
# For Ubuntu 14.04
set(Python_ADDITIONAL_VERSIONS 3.4)

#============================================================================
# Install dir.
#============================================================================
execute_process(COMMAND git rev-list HEAD --count
        OUTPUT_VARIABLE STRIDE_GIT_SHA OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_INSTALL_PREFIX  $ENV{HOME}/opt/stride-${STRIDE_GIT_SHA})

#============================================================================
# Compiler.
#============================================================================
if(LINUX)
    if(STRIDE_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILER   /usr/bin/gcc  CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /usr/bin/g++  CACHE PATH "CXX compiler path")
    elseif(STRIDE_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_C_COMPILER   /opt/clang/bin/clang    CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /opt/clang/bin/clang++  CACHE PATH "CXX compiler path")
    endif()
endif()
#
if(APPLE)
    if(STRIDE_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILER   /opt/local/bin/gcc  CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /opt/local/bin/g++  CACHE PATH "CXX compiler path")
    elseif(STRIDE_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_C_COMPILER   /opt/local/bin/clang   CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /opt/local/bin/clang++ CACHE PATH "CXX compiler path")
    elseif(STRIDE_COMPILER_ID STREQUAL "Apple")
        set(CMAKE_C_COMPILER   /usr/bin/cc   CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /usr/bin/c++  CACHE PATH "CXX compiler path")
    endif()
endif()

#============================================================================
# Boost.
#============================================================================
if(LINUX)
    set(STRIDE_BOOST_ROOT "/opt/boost/gcc/boost_1_66_0/")
    set(STRIDE_BOOST_NO_SYSTEM_PATHS ON)
endif()
if(APPLE AND STRIDE_COMPILER_ID STREQUAL "GNU")
    set(STRIDE_BOOST_ROOT "/opt/boost-1.66.0")
    set(STRIDE_BOOST_NO_SYSTEM_PATHS ON)
endif()

#############################################################################
