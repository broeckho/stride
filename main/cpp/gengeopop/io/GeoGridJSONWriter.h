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

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <set>

#include "GeoGridWriter.h"

namespace gengeopop {

/**
 * An implementation of the GeoGridWriter using JSON
 * This class is used to write a GeoGrid to a JSON file
 */
class GeoGridJSONWriter : public GeoGridWriter
{
public:
        /// Construct the GeoGridJSONWriter
        GeoGridJSONWriter();

        /// Write the provided GeoGrid to the proved ostream in JSON format
        void Write(std::shared_ptr<GeoGrid> geoGrid, std::ostream& stream) override;

private:
        /// Create a Boost Property Tree containing all the information needed to reconstruct a Location
        boost::property_tree::ptree WriteLocation(std::shared_ptr<Location> location);

        /// Create a Boost Property Tree containing all the information needed to reconstruct a Coordinate
        boost::property_tree::ptree WriteCoordinate(const Coordinate& coordinate);

        /// Create a Boost Property Tree containing all the information needed to reconstruct a ContactCenter
        boost::property_tree::ptree WriteContactCenter(std::shared_ptr<ContactCenter> contactCenter);

        /// Create a Boost Property Tree containing all the information needed to reconstruct a ContactPool
        boost::property_tree::ptree WriteContactPool(stride::ContactPool* contactPool);

        /// Create a Boost Property Tree containing all the information needed to reconstruct a Person
        boost::property_tree::ptree WritePerson(stride::Person* person);

        std::set<stride::Person*>
            m_persons_found; ///< The persons found when looping over the ContactPools.
                             ///< These are needed since they are stored in a separate list inside the JSON file.
};
} // namespace gengeopop
