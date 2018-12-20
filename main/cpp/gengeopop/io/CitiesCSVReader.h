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

#include "CitiesReader.h"
#include "util/CSV.h"

namespace gengeopop {

/**
 * Create a Reader which fills a GeoGrid with the Cities found in a given CSV file.
 */
class CitiesCSVReader : public CitiesReader
{
public:
        /// Construct the CitiesCSVReader with an istream containing the CSV data.
        explicit CitiesCSVReader(std::unique_ptr<std::istream> inputStream);

        /// Add the found Locations to the provided GeoGrid.
        void FillGeoGrid(std::shared_ptr<GeoGrid> geoGrid) const override;

private:
        stride::util::CSV m_reader; ///< The pure CSV reader
};

} // namespace
