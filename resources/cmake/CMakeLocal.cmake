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

#============================================================================
# MACRO (WARNING: CLion overrides the setting of CMAKE_BUILD_TYPE)
#============================================================================
set(GOBELIJN_INCLUDE_DOC      FALSE)
set(GOBELIJN_VERBOSE_TESTING  FALSE)
#set(GOBELIJN_COMPILER_ID      GNU)
set(GOBELIJN_COMPILER_ID      Clang)
#set(GOBELIJN_COMPILER_ID      Apple)

#============================================================================
# Install dir.
#============================================================================
execute_process(COMMAND git rev-list HEAD --count
        OUTPUT_VARIABLE STRIDE_GIT_LABEL OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_INSTALL_PREFIX  $ENV{HOME}/opt/stride-${STRIDE_GIT_LABEL})

#============================================================================
# Compiler.
#============================================================================
if(LINUX)
    if(GOBELIJN_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILER   /usr/bin/gcc  CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /usr/bin/g++  CACHE PATH "CXX compiler path")
    elseif(GOBELIJN_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_C_COMPILER   /opt/clang/bin/clang    CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /opt/clang/bin/clang++  CACHE PATH "CXX compiler path")
    endif()
endif()
#
if(APPLE)
    if(GOBELIJN_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILER   /opt/local/bin/gcc  CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /opt/local/bin/g++  CACHE PATH "CXX compiler path")
    elseif(GOBELIJN_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_C_COMPILER   /opt/local/bin/clang   CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /opt/local/bin/clang++ CACHE PATH "CXX compiler path")
    elseif(GOBELIJN_COMPILER_ID STREQUAL "Apple")
        set(CMAKE_C_COMPILER   /usr/bin/cc   CACHE PATH "C compiler path")
        set(CMAKE_CXX_COMPILER /usr/bin/c++  CACHE PATH "CXX compiler path")
    endif()
#    set(GOBELIJN_BOOST_ROOT "")
endif()

#############################################################################