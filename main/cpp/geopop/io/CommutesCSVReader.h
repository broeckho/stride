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

#include "CommutesReader.h"

#include <memory>

namespace geopop {

/**
 * Create a Reader that fills a GeoGrid with the commute info from CSV file.
 */
class CommutesCSVReader : public CommutesReader
{
public:
        /// Construct the CommutesCSVReader with an istream containing the CSV data.
        explicit CommutesCSVReader(std::unique_ptr<std::istream> istream);

        /// Add the found Commutes to the provided GeoGrid
        void FillGeoGrid(std::shared_ptr<GeoGrid> geoGrid) const override;
};

} // namespace geopop
