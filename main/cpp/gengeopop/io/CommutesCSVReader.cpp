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

#include "CommutesCSVReader.h"
#include "util/CSV.h"
#include "util/Exception.h"

#include <iostream>

namespace gengeopop {

CommutesCSVReader::CommutesCSVReader(std::unique_ptr<std::istream> inputStream)
    : CommutesReader(std::move(inputStream)), m_reader(*(m_inputStream.get()))
{
}

void CommutesCSVReader::FillGeoGrid(std::shared_ptr<GeoGrid> geoGrid) const
{
        // cols:
        // flanders_commuting format
        // kolom: stad van vertrek (headers = id)
        // rij: stad van aankomst (volgorde = volgorde van kolommen = id).

        // represents the location id for column x
        std::vector<unsigned int> headerMeaning;

        for (const std::string& label : m_reader.GetLabels()) {
                headerMeaning.push_back(static_cast<unsigned int>(stoi(label.substr(3))));
        }

        const size_t columnCount = m_reader.GetColumnCount();

        std::map<unsigned int, unsigned int> sizes; // indexed by header/row id

        // Since columns represent the "from city" and the proportion is calculated using the from city,
        // the total population of a city is calculated using the values found in the columns.
        for (const stride::util::CSVRow& row : m_reader) {
                for (size_t columnIndex = 0; columnIndex < columnCount; columnIndex++) {
                        sizes[columnIndex] += row.GetValue<int>(columnIndex);
                }
        }

        size_t rowIndex = 0;
        for (const stride::util::CSVRow& row : m_reader) {
                for (size_t columnIndex = 0; columnIndex < columnCount; columnIndex++) {
                        auto abs = row.GetValue<double>(columnIndex);
                        if (abs != 0 && columnIndex != rowIndex) {
                                const auto& locFrom    = geoGrid->GetById(headerMeaning[columnIndex]);
                                const auto& locTo      = geoGrid->GetById(headerMeaning[rowIndex]);
                                const auto& total      = sizes[columnIndex];
                                double      proportion = abs / total;

                                if (proportion < 0 || proportion > 1) {
                                        throw stride::util::Exception(
                                            "Proportion of commutes from " + std::to_string(locFrom->GetID()) + " to " +
                                            std::to_string(locTo->GetID()) + " is invalid (0 <= proportion <= 1)");
                                }
                                locFrom->AddOutgoingCommutingLocation(locTo, proportion);
                                locTo->AddIncomingCommutingLocation(locFrom, proportion);
                        }
                }
                rowIndex++;
        }
}

} // namespace
