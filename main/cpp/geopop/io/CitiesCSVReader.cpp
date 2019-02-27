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

#include "CitiesCSVReader.h"

#include "util/CSV.h"
#include "util/CSVRow.h"

namespace geopop {

using namespace std;
using namespace stride::util;

CitiesCSVReader::CitiesCSVReader(unique_ptr<istream> inputStream) : CitiesReader(move(inputStream)) {}

void CitiesCSVReader::FillGeoGrid(shared_ptr<GeoGrid> geoGrid) const
{
        vector<pair<shared_ptr<Location>, int>> addedLocations;

        CSV  reader(*(m_inputStream.get()));
        auto totalPopulation = 0U;

        for (const CSVRow& row : reader) {
                const auto id = row.GetValue<int>(0);
                // In file: id,province,population,x_coord,y_coord,latitude,longitude,name
                // Ignore x and y, we do not use them,
                // In Coordinate constructor switch order of latitude and longitude
                const auto loc = make_shared<Location>(id, row.GetValue<int>(1),
                                                       Coordinate(row.GetValue<double>(6), row.GetValue<double>(5)),
                                                       row.GetValue(7));
                geoGrid->AddLocation(loc);
                addedLocations.emplace_back(loc, row.GetValue<int>(2));
                totalPopulation += row.GetValue<int>(2);
        }

        for (const auto& l : addedLocations) {
                l.first->SetRelativePopulation(static_cast<double>(l.second) / static_cast<double>(totalPopulation));
        }
}

} // namespace geopop
