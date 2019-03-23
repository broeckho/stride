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

#include "json.hpp"

#include <boost/property_tree/ptree_fwd.hpp>

namespace geopop {

class GeoGrid;

/**
 * An implementation of the GeoGridReader using JSON.
 * This class is used to read a GeoGrid from a JSON file.
 */
class GeoGridJSONReader : public GeoGridReader
{
public:
        /// Construct the GeoGridJSONReader with the istream which contains the JSON.
        GeoGridJSONReader(std::unique_ptr<std::istream> inputStream, stride::Population* pop);

        /// No copy constructor.
        GeoGridJSONReader(const GeoGridJSONReader&) = delete;

        /// No copy assignement.
        GeoGridJSONReader operator=(const GeoGridJSONReader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        void Read() override;

private:
        /// Create a ContactCenter based on the information stored in the provided boost property tree.
        std::shared_ptr<ContactCenter> ParseContactCenter(nlohmann::json& contactCenter);

        /// Create a ContactCenter based on the information stored in the provided boost property tree.
        stride::ContactPool* ParseContactPool(nlohmann::json& contactPool, stride::ContactType::Id typeId);

        /// Create a Coordinate based on the information stored in the provided boost property tree.
        Coordinate ParseCoordinate(nlohmann::json& coordinate);

        /// Create a Location based on the information stored in the provided boost property tree.
        std::shared_ptr<Location> ParseLocation(nlohmann::json& location);

        /// Create a Person based on the information stored in the provided boost property tree.
        stride::Person* ParsePerson(nlohmann::json& person);
};

} // namespace geopop
