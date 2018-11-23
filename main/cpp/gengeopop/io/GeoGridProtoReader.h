/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#pragma once
#include <set>

#include "GeoGridReader.h"
#include "proto/geogrid.pb.h"

namespace gengeopop {

/**
 * An implementation of the GeoGridReader using Protocol Buffers
 * This class is used to read a GeoGrid from a Proto file
 */

class GeoGridProtoReader : public GeoGridReader
{
public:
        /// Construct the GeoGridJSONReader with the istream which contains the Protobuf information
        GeoGridProtoReader(std::unique_ptr<std::istream> inputStream, stride::Population* pop);

        /// Actually perform the read and return the GeoGrid
        std::shared_ptr<GeoGrid> Read() override;

        /// No copy constructor
        GeoGridProtoReader(const GeoGridProtoReader&) = delete;

        /// No copy assignement
        GeoGridProtoReader operator=(const GeoGridProtoReader&) = delete;

private:
        /// Create a Location based on the provided protobuf Location
        std::shared_ptr<Location> ParseLocation(const proto::GeoGrid_Location& protoLocation);

        /// Create a Coordinate based on the provided protobuf Coordinate
        Coordinate ParseCoordinate(const proto::GeoGrid_Location_Coordinate& protoCoordinate);

        /// Create a ContactCenter based on the provided protobuf ContactCenter
        std::shared_ptr<ContactCenter> ParseContactCenter(
            const proto::GeoGrid_Location_ContactCenter& protoContactCenter);

        /// Create a ContactPool based on the provided protobuf ContactPool
        stride::ContactPool* ParseContactPool(const proto::GeoGrid_Location_ContactCenter_ContactPool& protoContactPool,
                                              stride::ContactPoolType::Id                              typeId);

        /// Create a Person based on the provided protobuf Person
        stride::Person* ParsePerson(const proto::GeoGrid_Person& person);

        std::shared_ptr<GeoGrid> m_geoGrid; ///< The GeoGrid which is being built
};
} // namespace gengeopop
