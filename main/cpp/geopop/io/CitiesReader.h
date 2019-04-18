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

#include "geopop/GeoGrid.h"
#include "geopop/Location.h"

#include <map>
#include <memory>
#include <string>

namespace geopop {

/**
 * Create an abstract Reader which fills a GeoGrid with the cities found in file.
 * This can be implemented using different input types. Currently CSV is implemented.
 */
class CitiesReader
{
public:
        /// Construct the CitiesReader with an istream containing the file content.
        explicit CitiesReader(std::unique_ptr<std::istream> inputStream) : m_inputStream(std::move(inputStream)) {}

        /// Default destructor.
        virtual ~CitiesReader() = default;

        /// Add the locations to the GeoGrid.
        virtual void FillGeoGrid(GeoGrid&) const = 0;

protected:
        std::unique_ptr<std::istream> m_inputStream; ///< The istream with the file content.
};

} // namespace geopop
