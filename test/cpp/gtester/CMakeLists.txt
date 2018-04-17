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
#  Copyright 2017, Willem L, Kuylen E, Stijven S & Broeckhove J
#############################################################################

#============================================================================
# Config related to using Google test (src in resources/lib/gtest) for cpp
#============================================================================
set(GTEST_RESOURCE_DIR	   ${CMAKE_SOURCE_DIR}/test/resources/lib/gtest/)
include_directories(SYSTEM ${GTEST_RESOURCE_DIR})
add_definitions(-DGTEST_LANG_CXX11)

#============================================================================
# Build & install the (OpenMP enabled if OpenMP available) executable.
#============================================================================
set(EXEC gtester)
set(SRC
    main.cpp
    BatchRuns.cpp
    ScenarioData.cpp
    UnitSegmentedVector.cpp
)
if (OPENMP_FOUND)
    set(SRC ${SRC} ParallelRuns.cpp)
endif()

add_executable(${EXEC}   ${SRC} )
target_link_libraries( ${EXEC}    ${LIBS} gtest pthread libstride)
install(TARGETS ${EXEC}  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Define tests.
#============================================================================
add_test(NAME  ${EXEC}
    WORKING_DIRECTORY  ${TESTS_DIR}
    COMMAND   ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/${EXEC}
        --gtest_output=xml:gtester_all.xml
)

#============================================================================
# Clean up.
#============================================================================
unset(EXEC)
unset(SRC)

#############################################################################
