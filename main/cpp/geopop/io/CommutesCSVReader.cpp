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

#include "CommutesCSVReader.h"

#include "geopop/GeoGrid.h"
#include "geopop/Location.h"
#include "util/CSV.h"
#include "util/Exception.h"

#include <map>

namespace geopop {

using namespace std;
using namespace stride::util;

CommutesCSVReader::CommutesCSVReader(unique_ptr<istream> inputStream) : CommutesReader(move(inputStream)) {}

void CommutesCSVReader::FillGeoGrid(shared_ptr<GeoGrid> geoGrid) const
{
        // flanders_commuting format
        // kolom: stad van vertrek (headers = id)
        // rij: stad van aankomst (volgorde = volgorde van kolommen = id).
        CSV reader(*(m_inputStream.get()));

        // represents the location id for column x
        vector<unsigned int> header;

        for (const string& label : reader.GetLabels()) {
                header.push_back(static_cast<unsigned int>(stoi(label.substr(3))));
        }

        const size_t                    columnCount = reader.GetColumnCount();
        map<unsigned int, unsigned int> sizes; // indexed by header/row id

        // Since columns represent the "from city" and the proportion is calculated using the from city,
        // the total population of a city is calculated using the values found in the columns.
        for (const CSVRow& row : reader) {
                for (size_t columnIndex = 0; columnIndex < columnCount; columnIndex++) {
                        sizes[columnIndex] += row.GetValue<int>(columnIndex);
                }
        }

        size_t rowIndex = 0;
        for (const CSVRow& row : reader) {
                for (size_t columnIndex = 0; columnIndex < columnCount; columnIndex++) {
                        auto abs = row.GetValue<double>(columnIndex);
                        if (abs != 0 && columnIndex != rowIndex) {
                                const auto& locFrom    = geoGrid->GetById(header[columnIndex]);
                                const auto& locTo      = geoGrid->GetById(header[rowIndex]);
                                const auto& total      = sizes[columnIndex];
                                double      proportion = abs / total;

                                if (proportion < 0 || proportion > 1) {
                                        throw Exception("Proportion of commutes from " + to_string(locFrom->GetID()) +
                                                        " to " + to_string(locTo->GetID()) +
                                                        " is invalid (0 <= proportion <= 1)");
                                }
                                locFrom->AddOutgoingCommutingLocation(locTo, proportion);
                                locTo->AddIncomingCommutingLocation(locFrom, proportion);
                        }
                }
                rowIndex++;
        }
}

} // namespace geopop
