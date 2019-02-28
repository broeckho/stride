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
#  Copyright 2018, Willem L, Kuylen E, Stijven S & Broeckhove J
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
    BatchRuns.cpp
    ScenarioData.cpp
    #---
    UnitCsvTest.cpp
    UnitIsIterator.cpp
    UnitPtreeUtils.cpp
    UnitPtreeUtilsHelper.h
    UnitRnEngine.cpp
    UnitRnMan.cpp
    UnitSVIterator.cpp
    UnitSegmentedVector.cpp
    UnitSlice1.cpp
    UnitSlice2.cpp
    #---
    createlogger.h
    #---
    gengeopop/generators/CollegeGeneratorTest.cpp
    gengeopop/generators/CommunityGeneratorTest.cpp
    gengeopop/generators/HouseholdGeneratorTest.cpp
    gengeopop/generators/SchoolGeneratorTest.cpp
    gengeopop/generators/WorkplaceGeneratorTest.cpp
    #---
    gengeopop/io/CitiesCSVReaderTest.cpp
    gengeopop/io/CommutesCSVReaderTest.cpp
    gengeopop/io/GeoGridIOUtils.cpp
    gengeopop/io/GeoGridJSONReaderTest.cpp
    gengeopop/io/GeoGridJSONWriterTest.cpp
    gengeopop/io/GeoGridProtoReaderTest.cpp
    gengeopop/io/GeoGridProtoWriterTest.cpp
    gengeopop/io/HouseholdCSVReaderTest.cpp
    gengeopop/io/ReaderFactoryTest.cpp
    #---
    gengeopop/kdtree.cpp
    #---
    gengeopop/populators/CommunityPopulatorTests.cpp
    gengeopop/populators/CollegePopulatorTest.cpp
    gengeopop/populators/HouseholdPopulatorTest.cpp
    gengeopop/populators/K12SchoolPopulatorTest.cpp
    gengeopop/populators/WorkplacePopulatorTest.cpp
    gengeopop/populators/createGeogrid.cpp
    #---
    myhayai/DeliveryMan.cpp
    myhayai/MyhayaiTests.cpp
    myhayai/class_delivery.cpp
    myhayai/flex_delivery.cpp
    #---
    main.cpp
)

if (OPENMP_FOUND)
        set(SRC ${SRC} ParallelRuns.cpp)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/test/cpp)
add_executable(${EXEC} ${SRC})
target_link_libraries(${EXEC} gtest libmyhayai libstride ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS ${EXEC}  DESTINATION   ${BIN_INSTALL_LOCATION})
install(DIRECTORY testdata/ DESTINATION ${TESTS_INSTALL_LOCATION}/testdata/)

#============================================================================
# Define tests.
#============================================================================
add_test(NAME ${EXEC}
        WORKING_DIRECTORY ${TESTS_DIR}
        COMMAND ${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_LOCATION}/${EXEC}
        --gtest_output=xml:gtester_all.xml
)

#############################################################################
