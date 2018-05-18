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
#  This file contains the C++ compile & link configuration.
#  It is meant to be included in the src/CMakeLists.txt and
#  to provide identical C++ configuration for the main/cpp
#  and the test/ccp directories and subdirectories.
#
#############################################################################

#----------------------------------------------------------------------------
# Check CMAKE_BUILD_TYPE
#----------------------------------------------------------------------------
if( NOT (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "Debug"))
	message(FATAL_ERROR  "========> CMAKE_BUILD_TYPE HAS TO MATCH EITHER Release OR Debug.")
endif()

#----------------------------------------------------------------------------
# Compile flags
#----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#
include(ProcessorCount)
ProcessorCount(PROCCOUNT)
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -DPROCCOUNT=${PROCCOUNT}")
#
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wno-array-bounds")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast" )
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O0"   )
#
include_directories(${CMAKE_HOME_DIRECTORY}/main/cpp)

#----------------------------------------------------------------------------
# Platform dependent compile flags
#----------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_HOST_APPLE)
	add_definitions( -D__APPLE__ )
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")
#
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_HOST_APPLE )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument -Wno-self-assign")
	add_definitions(-D__extern_always_inline=inline)
#
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
endif()

#----------------------------------------------------------------------------
# Standard math lib
#----------------------------------------------------------------------------
set(LIBS   ${LIBS}   m)

#----------------------------------------------------------------------------
# Spdlog Library (logging)
#----------------------------------------------------------------------------
include_directories(SYSTEM ${CMAKE_HOME_DIRECTORY}/main/resources/lib/spdlog/include)

#----------------------------------------------------------------------------
# TCLAP
#----------------------------------------------------------------------------
include_directories(SYSTEM ${CMAKE_HOME_DIRECTORY}/main/resources/lib/tclap/include)

#----------------------------------------------------------------------------
# SHA1 hash code.
#----------------------------------------------------------------------------
include_directories(SYSTEM ${CMAKE_HOME_DIRECTORY}/main/resources/lib/sha1/include)
set(LIBS ${LIBS} sha1)

#----------------------------------------------------------------------------
# Tina's Random Number Generator (TRNG) library and paths
#----------------------------------------------------------------------------
include_directories(SYSTEM ${CMAKE_HOME_DIRECTORY}/main/resources/lib/trng-4.20/include)
set(LIBS ${LIBS} trng)

#----------------------------------------------------------------------------
# Boost
#----------------------------------------------------------------------------
set(BOOST_ROOT ${STRIDE_BOOST_ROOT})
set(Boost_NO_SYSTEM_PATHS ${STRIDE_BOOST_NO_SYSTEM_PATHS})
message(status ${Boost_ROOT})
find_package(Boost COMPONENTS filesystem thread date_time system REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
set(LIBS   ${LIBS} ${Boost_LIBRARIES})

#----------------------------------------------------------------------------
# OpenMP
#----------------------------------------------------------------------------
if(NOT STRIDE_FORCE_NO_OPENMP)
	if(NOT DEFINED HAVE_CHECKED_OpenMP)
	    set(HAVE_CHECKED_OpenMP  TRUE  CACHE  BOOL  "Have checked for OpenMP?" FORCE)
        find_package(OpenMP)
        if( OPENMP_FOUND )
            set(HAVE_FOUND_OpenMP  TRUE  CACHE  BOOL  "Have found OpenMP?" FORCE)
        else()
            set(HAVE_FOUND_OpenMP  FALSE  CACHE  BOOL  "Have found OpenMP?" FORCE)
	    endif()
    endif()
    if (HAVE_FOUND_OpenMP)
    	set(OPENMP_FOUND TRUE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    else()
    	# This is done to eliminate blank output of undefined CMake variables.
    	set(OPENMP_FOUND FALSE)
    endif()
else()
    # This is done to eliminate blank output of undefined CMake variables.
    set(OPENMP_FOUND FALSE)
    set(HAVE_CHECKED_OpenMP FALSE)
    set(HAVE_FOUND_OpenMP FALSE)
endif()
# If not found, use the dummy omp.
if(NOT OPENMP_FOUND)
    include_directories(${CMAKE_HOME_DIRECTORY}/main/resources/lib/domp/include)
endif()

#----------------------------------------------------------------------------
# Qt : not a definitive list of components yet; these are placeholders
#----------------------------------------------------------------------------
find_package(Qt5 COMPONENTS Core Gui PrintSupport Widgets Qml QUIET)
if (Qt5_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQt5_FOUND=true")
    set(QT_INCLUDES
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
        ${QtQml_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        )
	include_directories(SYSTEM ${QT_INCLUDES})
	add_definitions(
        ${Qt5Core_DEFINITIONS}
        ${Qt5Gui_DEFINITIONS}
        ${Qt5PrintSupport_DEFINITIONS}
        ${QtQml_DEFINITIONS}
        ${Qt5Widgets_DEFINITIONS}
    )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    set(QT_LIBRARIES
        ${Qt5Core_LIBRARIES}
        ${Qt5Gui_LIBRARIES}
        ${Qt5PrintSupport_LIBRARIES}
        ${QtQml_LIBRARIES}
        ${Qt5Widgets_LIBRARIES}
        )
    if( CMAKE_BUILD_TYPE MATCHES "Release" )
        add_definitions( -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT )
    endif()
    if( CMAKE_BUILD_TYPE MATCHES "Debug" )
        add_definitions( -DQDEBUG_OUTPUT )
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQt5_FOUND=false")
endif()

#----------------------------------------------------------------------------
# HDF5 Library
# Try to find the C variant of libhdf5, if found, USE_HDF5 is defined
# and passed to the compilers to allow compilation of selective features
# through preprocessor commands like #ifdef USE_HDF5 and friends.
# Additional defs are required on Ubuntu where lib are installed
# with hdf5 v1.6 as default behavior.
#----------------------------------------------------------------------------
if(STRIDE_FORCE_NO_HDF5)
	message(STATUS "---> Skipping HDF5, STRIDE_FORCE_NO_HDF5 set.")
else()
	find_package(HDF5 COMPONENTS C HL)
	if(HDF5_FOUND)
		include_directories(SYSTEM ${HDF5_INCLUDE_DIRS})
		set(LIBS   ${LIBS}   ${HDF5_LIBRARIES})
		add_definitions(-DUSE_HDF5 -DH5_NO_DEPRECATED_SYMBOLS)
		add_definitions(-DH5Dcreate_vers=2 -DH5Dopen_vers=2)
		add_definitions(-DH5Acreate_vers=2 -DH5Gcreate_vers=2)
		add_definitions(-DH5Gopen_vers=2 )
	else()
		# This is done to eliminate blank output of undefined CMake variables.
		set(HDF5_FOUND FALSE)
	endif()
endif()

#############################################################################
