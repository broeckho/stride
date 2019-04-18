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

message(STATUS "\nReading compiler settings from CMakeLocal.cmake! \n")

#============================================================================
# MACRO (WARNING: CLion overrides the setting of CMAKE_BUILD_TYPE)
#============================================================================
set(STRIDE_INCLUDE_DOC      OFF)
set(STRIDE_FORCE_NO_BOOST   OFF)
set(STRIDE_FORCE_NO_OPENMP  OFF)
set(STRIDE_FORCE_NO_PROTOC  OFF)
set(STRIDE_FORCE_NO_PYTHON  OFF)

if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(STRIDE_COMPILER_ID      GNU)
    #set(STRIDE_COMPILER_ID      Clang)
    #set(STRIDE_COMPILER_ID      Apple)
endif()
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

#============================================================================
# Compiler.
#============================================================================
if(LINUX)
    if(STRIDE_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILER   /usr/bin/gcc  CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /usr/bin/g++  CACHE PATH "CXX compiler path")
    elseif(STRIDE_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_C_COMPILER   /usr/bin/clang    CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /usr/bin/clang++  CACHE PATH "CXX compiler path")
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

#############################################################################
