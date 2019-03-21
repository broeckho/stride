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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "GeoGridReader.h"
#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "geopop/Location.h"

#include <set>

namespace proto {
class GeoGrid_Location;
class GeoGrid_Location_Coordinate;
class GeoGrid_Location_ContactCenter;
class GeoGrid_Location_ContactCenter_ContactPool;
class GeoGrid_Person;
} // namespace proto

namespace geopop {

/**
 * An implementation of the GeoGridReader using Protocol Buffers.
 * This class is used to read a GeoGrid from a Proto file.
 */
class GeoGridProtoReader : public GeoGridReader
{
public:
        /// Construct the GeoGridJSONReader with the istream which contains the Protobuf info.
        GeoGridProtoReader(std::unique_ptr<std::istream> inputStream, stride::Population* pop);

        /// No copy constructor.
        GeoGridProtoReader(const GeoGridProtoReader&) = delete;

        /// No copy assignement.
        GeoGridProtoReader operator=(const GeoGridProtoReader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        void Read() override;

private:
        /// Create a ContactCenter based on protobuf ContactCenter info.
        std::shared_ptr<ContactCenter> ParseContactCenter(
            const proto::GeoGrid_Location_ContactCenter& protoContactCenter);

        /// Create a ContactPool based on the provided protobuf ContactPool.
        stride::ContactPool* ParseContactPool(const proto::GeoGrid_Location_ContactCenter_ContactPool& protoContactPool,
                                              stride::ContactType::Id                                  typeId);

        /// Create a Coordinate based on the provided protobuf Coordinate.
        Coordinate ParseCoordinate(const proto::GeoGrid_Location_Coordinate& protoCoordinate);

        /// Create a Location based on  protobuf Location info.
        std::shared_ptr<Location> ParseLocation(const proto::GeoGrid_Location& protoLocation);

        /// Create a Person based on protobuf Person info.
        stride::Person* ParsePerson(const proto::GeoGrid_Person& person);
};

} // namespace geopop
