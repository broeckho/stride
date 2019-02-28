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
# Protocol Buffers.
#============================================================================
set(PROTO_DIR geopop/io/proto)
if(STRIDE_GEN_PROTO)
    include(FindProtobuf)
    find_package(Protobuf)
    if(${PROTOBUF_FOUND} AND NOT ${PROTOBUF_PROTOC_EXECUTABLE} STREQUAL "Protobuf_PROTOC_EXECUTABLE-NOTFOUND")
        file(GLOB proto_source "${PROTO_DIR}/*.proto")
        protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${proto_source})
        add_custom_target(proto
            COMMAND ${CMAKE_COMMAND} -E copy ${PROTO_HEADER} io/proto
            COMMAND ${CMAKE_COMMAND} -E copy ${PROTO_SRC} io/proto
            )
    endif()
else()
    file(GLOB PROTO_HEADER "${PROTO_DIR}/*.pb.h")
    file(GLOB PROTO_SRC "${PROTO_DIR}/*.pb.cc")
endif()

set_property(SOURCE ${PROTO_HEADER} ${PROTO_SRC}
    PROPERTY
    COMPILE_FLAGS "-Wno-all -Wno-effc++ -Wno-extra -Wno-pedantic")

#============================================================================
# Sources.
#============================================================================
set(STRIDE_SRC
    #---
    calendar/Calendar.cpp
    #---
    contact/AgeContactProfile.cpp
    contact/ContactLogMode.cpp
    contact/ContactPool.cpp
    contact/ContactType.cpp
    contact/Infector.cpp
    contact/TransmissionProfile.cpp
    #---
    disease/DiseaseSeeder.cpp
    disease/Health.cpp
    disease/HealthSeeder.cpp
    disease/Immunizer.cpp
    disease/PublicHealthAgency.cpp
    #---
    execs/ControlHelper.cpp
    execs/GenPopController.cpp
    execs/SimController.cpp
    execs/StanController.cpp
    #---
    geopop/College.cpp
    geopop/ContactCenter.cpp
    geopop/Coordinate.h
    geopop/GeoGrid.cpp
    geopop/GeoGridBuilder.cpp
    geopop/GeoGridConfig.cpp
    geopop/GeoGridConfigBuilder.cpp
    geopop/Household.cpp
    geopop/K12School.cpp
    geopop/Location.cpp
    geopop/PrimaryCommunity.cpp
    geopop/SecondaryCommunity.cpp
    geopop/Workplace.cpp
    #---
    geopop/geo/GeoGridKdTree.cpp
    geopop/geo/KdTree2DPoint.cpp
    #---
    geopop/generators/CollegeGenerator.cpp
    geopop/generators/CommunityGenerator.cpp
    geopop/generators/HouseholdGenerator.cpp
    geopop/generators/K12SchoolGenerator.cpp
    geopop/generators/WorkplaceGenerator.cpp
    #---
    geopop/io/CitiesCSVReader.cpp
    geopop/io/CommutesCSVReader.cpp
    geopop/io/GeoGridJSONReader.cpp
    geopop/io/GeoGridJSONWriter.cpp
    geopop/io/GeoGridProtoReader.cpp
    geopop/io/GeoGridProtoWriter.cpp
    geopop/io/GeoGridReader.cpp
    geopop/io/GeoGridReaderFactory.cpp
    geopop/io/GeoGridWriterFactory.cpp
    geopop/io/HouseholdCSVReader.cpp
    geopop/io/ReaderFactory.cpp
    geopop/populators/CollegePopulator.cpp
    geopop/populators/HouseholdPopulator.cpp
    geopop/populators/K12SchoolPopulator.cpp
    geopop/populators/Populator.cpp
    geopop/populators/PrimaryCommunityPopulator.cpp
    geopop/populators/SecondaryCommunityPopulator.cpp
    geopop/populators/WorkplacePopulator.cpp
    #---
    ${PROTO_HEADER}
    ${PROTO_SRC}
    #---
    viewers/InfectedFile.cpp
    viewers/PersonsFile.cpp
    viewers/SummaryFile.cpp
    #---
    pop/Person.cpp
    pop/Population.cpp
    pop/DefaultPopBuilder.cpp
    pop/GeoPopBuilder.cpp
    pop/ImportPopBuilder.cpp
    pop/SurveySeeder.cpp
    #---
    sim/SimRunner.cpp
    sim/Sim.cpp
    sim/SimBuilder.cpp
    sim/event/Id.cpp
    sim/event/Subject.cpp
    #---
    util/CSVRow.cpp
    util/LogUtils.cpp
    util/Rn.cpp
    util/CSV.cpp
    util/ConfigInfo.cpp
    util/FileSys.cpp
    util/RunConfigManager.cpp
    #---
    viewers/InfectedFileViewer.cpp
    viewers/InfectedViewer.cpp
    viewers/CliViewer.cpp
    viewers/PersonsFileViewer.cpp
    viewers/SummaryFileViewer.cpp
)

#============================================================================
# MACROS to inject in ConfigInfo.cpp
#============================================================================
set_source_files_properties(util/ConfigInfo.cpp PROPERTIES
    COMPILE_DEFINITIONS "STRIDE_GIT_HASH=${STRIDE_WC_REVISION_HASH}")
set_source_files_properties(util/ConfigInfo.cpp PROPERTIES
    COMPILE_DEFINITIONS "PROCCOUNT=${PROCCOUNT}")

#============================================================================
# Build the Stride library.
#============================================================================
add_library(libstride ${STRIDE_SRC})
target_link_libraries(libstride ${LIBS} libprotobuf_insrc)
set_target_properties(libstride
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        PREFIX ""
)
include(CheckIPOSupported)

# For future use: inter procedural optimization
check_ipo_supported(RESULT ipo_result)
if(ipo_result)
    set_target_properties(libstride PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
message( STATUS "CheckIPOSupported: ${ipo_result}" )

#============================================================================
# Build the stride binary.
#============================================================================
add_executable(stride execs/stride.cpp)
target_link_libraries(stride libstride)
install(TARGETS stride DESTINATION ${BIN_INSTALL_LOCATION})

#############################################################################
