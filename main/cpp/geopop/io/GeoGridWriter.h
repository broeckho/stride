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

#include <memory>
#include <ostream>

namespace geopop {

class GeoGrid;

/**
 * An interface for writing the GeoGrid to a file, can be implemented with multiple file types.
 * Protobuf and json are currently implemented.
 */
class GeoGridWriter
{
public:
        /// Construct the Writer.
        virtual ~GeoGridWriter() = default;

        /// Write the GeoGrid to ostream.
        virtual void Write(GeoGrid& geoGrid, std::ostream& stream) = 0;
};

} // namespace geopop
