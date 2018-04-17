#############################################################################
#  This is free software: you can redistribute it and/or modify
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

#============================================================================
# Make test directory in install tree:
#============================================================================
set(TESTS_DIR ${CMAKE_INSTALL_PREFIX}/${TESTS_INSTALL_LOCATION})
install(DIRECTORY DESTINATION  ${TESTS_DIR})

#============================================================================
# Add subdirectories:
#============================================================================
add_subdirectory(cpp)
if (${SWIG_FOUND} AND ${PYTHONLIBS_FOUND})
	add_subdirectory(python)
endif()
add_subdirectory(resources)

#############################################################################
