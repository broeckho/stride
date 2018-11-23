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

#include "CommutesReader.h"
#include <gengeopop/Location.h>
#include <map>
#include <string>
#include <util/CSV.h>

namespace gengeopop {

/**
 * Create a Reader which fills a GeoGrid with the Commutes found in a given CSV file
 */
class CommutesCSVReader : public CommutesReader
{
public:
        /// Construct the CommutesCSVReader with an istream containing the CSV data
        explicit CommutesCSVReader(std::unique_ptr<std::istream> istream);

        /// Add the found Commutes to the provided GeoGrid
        void FillGeoGrid(std::shared_ptr<GeoGrid> geoGrid) const override;

private:
        /// Add the provided commute symmetrically to the locations
        void AddCommute(std::shared_ptr<Location> from, std::shared_ptr<Location> to, double proportion) const;

        stride::util::CSV m_reader; ///< The pure CSV reader
};
} // namespace gengeopop
