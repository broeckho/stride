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
#  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
#############################################################################

#============================================================================
# Sources.
#============================================================================
set(GENGEOPOP_LIB
        Community.cpp
        PrimaryCommunity.cpp
        SecondaryCommunity.cpp
        ContactCenter.cpp
        GenGeoPopController.cpp
        GeoGrid.cpp
        College.cpp
        Household.cpp
        Location.cpp
        K12School.cpp
        Workplace.cpp
        GeoGridConfig.cpp
#
        generators/GeoGridGenerator.cpp
        generators/K12SchoolGenerator.cpp
        generators/PartialGenerator.cpp
        generators/CollegeGenerator.cpp
        generators/HouseholdGenerator.cpp
        generators/WorkplaceGenerator.cpp
        generators/CommunityGenerator.cpp
#
        populators/PrimaryCommunityPopulator.cpp
        populators/SecondaryCommunityPopulator.cpp
        populators/GeoGridPopulator.cpp
        populators/HouseholdPopulator.cpp
        populators/PartialPopulator.cpp
        populators/K12SchoolPopulator.cpp
        populators/CollegePopulator.cpp
        populators/WorkplacePopulator.cpp
#
        io/CitiesCSVReader.cpp
        io/CommutesCSVReader.cpp
        io/HouseholdCSVReader.cpp
        io/HouseholdReader.cpp
        io/ReaderFactory.cpp
        io/GeoGridJSONWriter.cpp
        io/GeoGridJSONReader.cpp
        io/GeoGridProtoWriter.cpp
        io/GeoGridProtoReader.cpp
        io/GeoGridWriterFactory.cpp
        io/GeoGridReaderFactory.cpp
        io/GeoGridReader.cpp
)

#============================================================================
# Protocol Buffers.
#============================================================================
if(STRIDE_GEN_PROTO)
        include(FindProtobuf)
        find_package(Protobuf)
        if(${PROTOBUF_FOUND} AND NOT ${PROTOBUF_PROTOC_EXECUTABLE} STREQUAL "Protobuf_PROTOC_EXECUTABLE-NOTFOUND")
            file(GLOB proto_source "${CMAKE_CURRENT_SOURCE_DIR}/io/proto/*.proto")
            protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${proto_source})
            add_custom_target(proto
                COMMAND ${CMAKE_COMMAND} -E copy ${PROTO_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/io/proto/
                COMMAND ${CMAKE_COMMAND} -E copy ${PROTO_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/io/proto/
            )
        endif()
else()
        file(GLOB PROTO_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/io/proto/*.pb.h")
        file(GLOB PROTO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/io/proto/*.pb.cc")
endif()
add_library(libproto ${PROTO_HEADER} ${PROTO_SRC})
target_link_libraries(libproto libprotobuf_insrc)

set_target_properties(libproto PROPERTIES COMPILE_FLAGS "-Wno-all -Wno-effc++ -Wno-extra -Wno-pedantic")
set_property(TARGET libproto PROPERTY POSITION_INDEPENDENT_CODE ON)

#============================================================================
# Build the GenGeoPop library.
#============================================================================
add_library(libgengeopop ${GENGEOPOP_LIB})
target_link_libraries(libgengeopop ${LIBS} libprotobuf_insrc libproto libstride)
set_property(TARGET libgengeopop PROPERTY POSITION_INDEPENDENT_CODE ON)

#============================================================================
# Clean up.
#============================================================================
unset(GENGEOPOP_LIB)

#############################################################################
