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

#include "GeoGridWriter.h"
#include "geopop/Location.h"

#include <set>

namespace stride {
class ContactPool;
class Person;
} // namespace stride

namespace proto {
class GeoGrid_Location;
class GeoGrid_Location_Coordinate;
class GeoGrid_Location_ContactPools;
class GeoGrid_Location_ContactPools_ContactPool;
class GeoGrid_Person;
} // namespace proto

namespace geopop {

/**
 * An implementation of the GeoGridWriter using Protocol Buffers
 * This class is used to write a GeoGrid to a Proto file
 */
class GeoGridProtoWriter : public GeoGridWriter
{
public:
        /// Construct the GeoGridProtoWriter.
        GeoGridProtoWriter();

        /// Write the GeoGrid to the ostream in Protobuf format.
        void Write(GeoGrid& geoGrid, std::ostream& stream) override;

private:
        /// Create a ProtoBuf ContactPools structure.
        void WriteContactPools(stride::ContactType::Id                              typeId,
                               stride::util::SegmentedVector<stride::ContactPool*>& contactPools,
                               proto::GeoGrid_Location_ContactPools*                protoContactPools);

        /// Create a ProtoBuf ContactPool containing all the info needed to reconstruct a ContactPool.
        void WriteContactPool(stride::ContactPool*                              contactPool,
                              proto::GeoGrid_Location_ContactPools_ContactPool* protoContactPool);

        /// Create a ProtoBuf Coordinate containing all the info needed to reconstruct a Coordinate..
        void WriteCoordinate(const Coordinate& coordinate, proto::GeoGrid_Location_Coordinate* protoCoordinate);

        /// Create a ProtoBuf Location containing all the info needed to reconstruct a Location.
        void WriteLocation(Location& location, proto::GeoGrid_Location* protoLocation);

        /// Create a ProtoBuf Person containing all the info needed to reconstruct a Person.
        void WritePerson(stride::Person* person, proto::GeoGrid_Person* protoPerson);

private:
        std::set<stride::Person*> m_persons_found; ///< The persons found when looping over the ContactPools.
};

} // namespace geopop
