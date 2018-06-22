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
#  Copyright 2017, 2018 Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# Define smoke test. DO NOT BREAK THE COMMAND LINE.
#============================================================================
add_test(NAME smoke
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
    COMMAND ${BIN_INSTALL_LOCATION}/stride --override output_prefix=tests/smoke_test
	)

#============================================================================
# Add subdirectories:
#============================================================================
add_subdirectory(btester)
add_subdirectory(gtester)

#############################################################################
