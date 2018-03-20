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
# Example scripts
#================================================================================================
add_subdirectory(examples)

#============================================================================
# Build & install the Swig files for python environment.
#============================================================================
set(SWIG_IFACE
    swig/contactpooltype.i
	swig/health.i
	swig/population.i
	swig/simulator.i
	swig/simulatorobserver.i
	swig/striderunner.i
	swig/stride.i
)

include(${SWIG_USE_FILE})
include_directories(${PYTHON_INCLUDE_DIRS})
set_property(SOURCE ${SWIG_IFACE} PROPERTY CPLUSPLUS ON)
swig_add_library(stride LANGUAGE python SOURCES swig/stride.i TYPE MODULE)
swig_link_libraries(stride PUBLIC libstride ${PYTHON_LIBRARIES})

#================================================================================================
# Install Python package.
#================================================================================================
SET(PYTHON_TO_INSTALL _pythonInstallPyStride)

add_custom_target(pythonInstallPyStride ALL
				  DEPENDS
				  _stride
				  ${PYTHON_TO_INSTALL}
)

add_custom_command(OUTPUT _pythonInstallPyStride
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/stride pystride/stride
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/main/python/stride.py pystride/stride/stride.py
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/main/python/_stride.so pystride/stride/_stride.so
)

#================================================================================================
# Copy the Python scripts to the bin folder.
#================================================================================================
INSTALL(DIRECTORY pystride
        DESTINATION ${BIN_INSTALL_LOCATION})
INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/main/python/pystride/stride
        DESTINATION ${BIN_INSTALL_LOCATION}/pystride/)

#================================================================================================
# Clean up
#================================================================================================
unset(SWIG_IFACE)

#################################################################################################
