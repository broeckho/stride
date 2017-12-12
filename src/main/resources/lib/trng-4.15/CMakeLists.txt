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
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#############################################################################
#
#  Build trng.
#
#############################################################################

#============================================================================
# Build Google test framework gtest (build only, no install)
#============================================================================
set( SRC
	src/lcg64.cc
	src/lcg64_shift.cc
	src/minstd.cc
	src/mrg2.cc
	src/mrg3.cc
	src/mrg3s.cc
	src/mrg4.cc
	src/mrg5.cc
	src/mrg5s.cc
	src/mt19937.cc
	src/mt19937_64.cc
	src/yarn2.cc
	src/yarn3.cc
	src/yarn3s.cc
	src/yarn4.cc
	src/yarn5.cc
	src/yarn5s.cc
)

add_library( trng ${SRC} )
#target_compile_options(trng PUBLIC "-flto")
unset( SRC )	

#############################################################################
