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
set_source_files_properties(
    geopop/io/GeoGridIOUtils.cpp
    geopop/io/GeoGridProtoReaderTest.cpp
    PROPERTIES
    COMPILE_FLAGS "-I${Protobuf_PBS_DIR}"
)

set(EXEC gtester)
set(SRC
    ScenarioData.cpp
    ScenarioRuns.cpp
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
    #---
    geopop/generators/CollegeGeneratorTest.cpp
    geopop/generators/HouseholdGeneratorTest.cpp
    geopop/generators/K12SchoolGeneratorTest.cpp
    geopop/generators/PrimaryCommunityGeneratorTest.cpp
    geopop/generators/SecondaryCommunityGeneratorTest.cpp
    geopop/generators/WorkplaceGeneratorTest.cpp
    #---
    geopop/io/CitiesCSVReaderTest.cpp
    geopop/io/CommutesCSVReaderTest.cpp
    geopop/io/GeoGridIOUtils.cpp
    #geopop/io/GeoGridJSONReaderTest.cpp
    #geopop/io/GeoGridJSONWriterTest.cpp
    geopop/io/GeoGridProtoReaderTest.cpp
    geopop/io/GeoGridProtoWriterTest.cpp
    geopop/io/HouseholdCSVReaderTest.cpp
    geopop/io/ReaderFactoryTest.cpp
    #---
    geopop/kdtree.cpp
    #---
    geopop/populators/MakeGeoGrid.cpp
    geopop/populators/CollegePopulatorTest.cpp
    geopop/populators/HouseholdPopulatorTest.cpp
    geopop/populators/K12SchoolPopulatorTest.cpp
    geopop/populators/PrimaryCommunityPopulatorTests.cpp
    geopop/populators/SecondaryCommunityPopulatorTests.cpp
    geopop/populators/WorkplacePopulatorTest.cpp
    #---
    myhayai/DeliveryMan.cpp
    myhayai/MyhayaiTests.cpp
    myhayai/class_delivery.cpp
    myhayai/flex_delivery.cpp
    #---
    main.cpp
)

#if (OPENMP_FOUND)
#        set(SRC ${SRC} ParallelRuns.cpp)
#endif ()

include_directories(${CMAKE_SOURCE_DIR}/test/cpp)
add_executable(${EXEC} ${SRC})
target_link_libraries(${EXEC} gtest libmyhayai libstride ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS ${EXEC}  DESTINATION   ${BIN_INSTALL_LOCATION})
install(DIRECTORY testdata/ DESTINATION ${TESTS_INSTALL_LOCATION}/testdata/)

#============================================================================
# Define tests.
#============================================================================
add_test(NAME ${EXEC}
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
        COMMAND ${BIN_INSTALL_LOCATION}/${EXEC} --gtest_output=xml:${TESTS_INSTALL_LOCATION}/gtester_all.xml
)

#############################################################################
