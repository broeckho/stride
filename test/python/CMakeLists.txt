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
set (SCRIPTS
	testForker.py
	testInfected.py
    testPrint.py
	testSmoke.py
	testVaccinate.py
)

INSTALL (FILES
	${SCRIPTS}
	DESTINATION ${BIN_INSTALL_LOCATION}
)

#============================================================================
# Define tests.
#============================================================================
add_test(NAME infected_py
	WORKING_DIRECTORY ${TESTS_DIR}
	COMMAND python3 ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/testInfected.py
	)
add_test(NAME forker_py
	WORKING_DIRECTORY ${TESTS_DIR}
	COMMAND python3 ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/testForker.py
	)
add_test(NAME print_py
    WORKING_DIRECTORY ${TESTS_DIR}
    COMMAND python3 ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/testPrint.py
    )
add_test(NAME smoke_py
	WORKING_DIRECTORY ${TESTS_DIR}
	COMMAND python3 ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/testSmoke.py
	)
add_test(NAME vaccinate_py
	WORKING_DIRECTORY ${TESTS_DIR}
	COMMAND  python3 ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/testVaccinate.py
	)

#============================================================================
# Clean up.
#============================================================================
unset(SCRIPTS)

#############################################################################
