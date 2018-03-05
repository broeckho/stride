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
# Test verbosity:
#============================================================================
unset( CTEST_VERBOSE_OPTION )
if( STRIDE_VERBOSE_TESTING )
	set( CTEST_VERBOSE_OPTION "-V" )
endif()

#============================================================================
# Make test directory in install tree:
#============================================================================
set( TESTS_DIR ${CMAKE_INSTALL_PREFIX}/${TESTS_INSTALL_LOCATION} )
install(DIRECTORY DESTINATION  ${TESTS_DIR} )

#============================================================================
# Top level custom target for instactive use through make:
#============================================================================
add_custom_target( run_ctest
	COMMAND ctest ${CTEST_VERBOSE_OPTION} -O ${TESTS_DIR}/ctests.txt
)

#============================================================================
# Config related to using Google test (src in resources/lib/gtest) for cpp
#============================================================================
set( GTEST_RESOURCE_DIR	   ${CMAKE_SOURCE_DIR}/test/resources/lib/gtest/ )
include_directories( ${GTEST_RESOURCE_DIR} )
add_definitions( -DGTEST_LANG_CXX11 )

#============================================================================
# Add subdirectories:
#============================================================================
add_subdirectory( cpp       )
add_subdirectory( python    )
add_subdirectory( resources )

#############################################################################
