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

#include "json.hpp"

#include <boost/property_tree/ptree.hpp>
#include <set>

namespace stride {
class ContactPool;
class Person;
} // namespace stride

namespace geopop {

class ContactPool;

/**
 * Writes a GeoGrid to a JSON file.
 */
class GeoGridJSONWriter : public GeoGridWriter
{
public:
        /// Construct the GeoGridJSONWriter.
        GeoGridJSONWriter();

        /// Write the provided GeoGrid to the proved ostream in JSON format.
        void Write(GeoGrid& geoGrid, std::ostream& stream) override;

private:
        /// Create a Boost Property Tree containing all info needed to reconstruct a ContactCenter.
        nlohmann::json WriteContactPools(stride::ContactType::Id typeId,
                stride::util::SegmentedVector<stride::ContactPool*>& pools);

        /// Create a Boost Property Tree containing all info needed to reconstruct a ContactPool.
        nlohmann::json WriteContactPool(stride::ContactPool* contactPool);

        /// Create a Boost Property Tree containing all info needed to reconstruct a Coordinate.
        nlohmann::json WriteCoordinate(const Coordinate& coordinate);

        /// Create a Boost Property Tree containing all info needed to reconstruct a Location.
        nlohmann::json WriteLocation(std::shared_ptr<Location> location);

        /// Create a Boost Property Tree containing all info needed to reconstruct a Person.
        nlohmann::json WritePerson(stride::Person* person);

private:
        std::set<stride::Person*> m_persons_found; ///< The persons found when looping over the ContactPools.
};

} // namespace geopop
