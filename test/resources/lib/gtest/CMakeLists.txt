#############################################################################
#  This file is part of the indismo software. 
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
#  Copyright 2014, L.Willem, S.Stijven, P.Beutels, N.Hens & J.Broeckhove, UA.
#############################################################################

#============================================================================
# Build Google test framework gtest (build only, no install)
#============================================================================
set(GTEST_RESOURCE_DIR	   ${CMAKE_SOURCE_DIR}/test/resources/lib/gtest/)
#
set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} -w")
include_directories(SYSTEM ${GTEST_RESOURCE_DIR})
add_definitions(-DGTEST_LANG_CXX11)
add_library(gtest STATIC ${GTEST_RESOURCE_DIR}/src/gtest-all.cc)

#############################################################################
