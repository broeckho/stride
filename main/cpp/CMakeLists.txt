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
# Protocol Buffer sources.
#============================================================================
if(Protobuf_FOUND)
    protobuf_generate_cpp(PROTO_SRC PROTO_HDR geopop/io/proto/geogrid.proto)
else()
    set(PROTO_SRC ${Protobuf_PBS_DIR}/geogrid.pb.cc)
    set(PROTO_HDR ${Protobuf_PBS_DIR}/geogrid.pb.h)
endif()

set_source_files_properties(
    geopop/io/GeoGridProtoReader.cpp
    geopop/io/GeoGridProtoWriter.cpp
    PROPERTIES
    COMPILE_FLAGS "-I${Protobuf_PBS_DIR}"
)

set_property(SOURCE ${PROTO_HDR} ${PROTO_SRC}
    PROPERTY
    COMPILE_FLAGS "-Wno-all -Wno-effc++ -Wno-extra -Wno-pedantic"
)

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
    geopop/io/GeoGridProtoReader.h
    geopop/io/GeoGridProtoWriter.cpp
    geopop/io/GeoGridProtoWriter.h
    geopop/io/GeoGridReader.cpp
    geopop/io/GeoGridReaderFactory.cpp
    geopop/io/GeoGridReaderFactory.h
    geopop/io/GeoGridWriterFactory.cpp
    geopop/io/GeoGridWriterFactory.h
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
    util/TimeStamp.cpp
    util/TimeToString.cpp
    #---
    viewers/InfectedFileViewer.cpp
    viewers/InfectedViewer.cpp
    viewers/CliViewer.cpp
    viewers/PersonsFileViewer.cpp
    viewers/SummaryFileViewer.cpp
)

#============================================================================
# Build the Stride library.
#============================================================================
add_library(libstride ${STRIDE_SRC})
target_compile_definitions(libstride PUBLIC "-DSTRIDE_GIT_HASH=${STRIDE_WC_REVISION_HASH}")
target_compile_definitions(libstride PUBLIC "-DPROCCOUNT=${PROCCOUNT}")
target_link_libraries(libstride PRIVATE ${LIBS})
if(Protobuf_FOUND)
    target_link_libraries(libstride PRIVATE ${Protobuf_LIBRARIES})
else()
    target_link_libraries(libstride PRIVATE libprotobuf)
endif()
set_target_properties(libstride
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        PREFIX ""
)
include(CheckIPOSupported)

# For future use: inter procedural optimization
# Disabled for now because of issues with gcc 8.1.0 on travis.
#check_ipo_supported(RESULT ipo_result)
#if(ipo_result)
#    set_target_properties(libstride PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
#endif()
#message( STATUS "CheckIPOSupported: ${ipo_result}" )

#============================================================================
# Build the stride binary.
#============================================================================
add_executable(stride execs/stride.cpp)
target_link_libraries(stride libstride)
install(TARGETS stride DESTINATION ${BIN_INSTALL_LOCATION})

#############################################################################
