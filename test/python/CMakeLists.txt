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
#  see http://www.gnu.org/licenses/
#
#  Copyright 2017, Willem L, Kuylen E, Stijven S & Broeckhove J
#############################################################################

#============================================================================
# Install the python test scripts.
#============================================================================
set ( SCRIPTS
	testSimple.py
)

INSTALL ( FILES
	${SCRIPTS}
	DESTINATION ${BIN_INSTALL_LOCATION}
)

#============================================================================
# Define tests.
#============================================================================
add_test( NAME python_environment
	WORKING_DIRECTORY ${TESTS_DIR}
	COMMAND python3 ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/testSimple.py
)

#============================================================================
# Clean up.
#============================================================================
unset( SCRIPTS   )

#############################################################################