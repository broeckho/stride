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
#  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
#############################################################################

#============================================================================
# Protocol Buffers.
#============================================================================
if(STRIDE_GEN_PROTO)
    include(FindProtobuf)
    find_package(Protobuf)
    if(${PROTOBUF_FOUND} AND NOT ${PROTOBUF_PROTOC_EXECUTABLE} STREQUAL "Protobuf_PROTOC_EXECUTABLE-NOTFOUND")
        file(GLOB proto_source "../geopop/io/proto/*.proto")
        protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${proto_source})
        add_custom_target(proto
            COMMAND ${CMAKE_COMMAND} -E copy ${PROTO_HEADER} io/proto
            COMMAND ${CMAKE_COMMAND} -E copy ${PROTO_SRC} io/proto
            )
    endif()
else()
    file(GLOB PROTO_HEADER "../geopop/io/proto/*.pb.h")
    file(GLOB PROTO_SRC "../geopop/io/proto/*.pb.cc")
endif()

set_property(SOURCE ${PROTO_HEADER} ${PROTO_SRC}
    PROPERTY
    COMPILE_FLAGS "-Wno-all -Wno-effc++ -Wno-extra -Wno-pedantic")

#============================================================================
# Sources.
#============================================================================
set(GENGEOPOP_SRC
#
    College.cpp
    ContactCenter.cpp
    Coordinate.h
    GeoGrid.cpp
    GeoGridBuilder.cpp
    GeoGridConfig.cpp
    GeoGridConfigBuilder.cpp
    Household.cpp
    K12School.cpp
    Location.cpp
    PrimaryCommunity.cpp
    SecondaryCommunity.cpp
    Workplace.cpp
#
    geo/GeoGridKdTree.cpp
    geo/KdTree2DPoint.cpp
#
    generators/CollegeGenerator.cpp
    generators/CommunityGenerator.cpp
    generators/HouseholdGenerator.cpp
    generators/K12SchoolGenerator.cpp
    generators/WorkplaceGenerator.cpp
#
    populators/CollegePopulator.cpp
    populators/HouseholdPopulator.cpp
    populators/K12SchoolPopulator.cpp
    populators/Populator.cpp
    populators/PrimaryCommunityPopulator.cpp
    populators/SecondaryCommunityPopulator.cpp
    populators/WorkplacePopulator.cpp
#
    io/CitiesCSVReader.cpp
    io/CommutesCSVReader.cpp
    io/GeoGridJSONReader.cpp
    io/GeoGridJSONWriter.cpp
    io/GeoGridProtoReader.cpp
    io/GeoGridProtoWriter.cpp
    io/GeoGridReader.cpp
    io/GeoGridReaderFactory.cpp
    io/GeoGridWriterFactory.cpp
    io/HouseholdCSVReader.cpp
    io/ReaderFactory.cpp
#
    ${PROTO_HEADER}
    ${PROTO_SRC}
)

message(STATUS "HAHAHAHA ${GENGEOPOP_SRC}")

#############################################################################
