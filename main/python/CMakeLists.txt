#################################################################################################
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
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#################################################################################################

#================================================================================================
# Example scripts + Scripts for 'Measles in Flanders' study
#================================================================================================
add_subdirectory(examples)
add_subdirectory(measles_in_flanders)
add_subdirectory(age_immunity_shift)

#================================================================================================
# Create the library that performs as a wrapper for some C++ code that is incompatible with SWIG.
#================================================================================================
set(SWIG_WRAPPER
    swig/cpp/create_sim.cpp
)
set_source_files_properties( ${SWIG_WRAPPER} PROPERTIES COMPILE_FLAGS "-fPIC")
add_library(swigwrapper ${SWIG_WRAPPER})

#============================================================================
# Build & install the Swig files for python environment.
#============================================================================
set(SWIG_IFACE
    swig/contactpooltype.i
	swig/health.i
	swig/population.i
	swig/sim.i
	swig/pystride.i
	swig/transmissionprofile.i
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
include(${SWIG_USE_FILE})
include_directories(${PYTHON_INCLUDE_DIRS})
set_property(SOURCE ${SWIG_IFACE} PROPERTY CPLUSPLUS ON)
swig_add_library(pystride LANGUAGE python SOURCES swig/pystride.i TYPE MODULE)
swig_link_libraries(pystride PUBLIC libstride swigwrapper ${PYTHON_LIBRARIES})

#================================================================================================
# Install Python package (modules plus scripts).
#================================================================================================
INSTALL(FILES
			${CMAKE_BINARY_DIR}/main/python/pystride.py
			${CMAKE_BINARY_DIR}/main/python/_pystride.so
		DESTINATION ${BIN_INSTALL_LOCATION}/pystride/stride)

INSTALL(DIRECTORY
			pystride
			postprocessing
        DESTINATION ${BIN_INSTALL_LOCATION})

#================================================================================================
# Clean up
#================================================================================================
unset(SWIG_IFACE)

#################################################################################################
