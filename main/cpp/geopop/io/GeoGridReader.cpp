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

#include "GeoGridReader.h"

#include "geopop/GeoGrid.h"
#include "geopop/Location.h"

namespace geopop {

GeoGridReader::GeoGridReader(std::unique_ptr<std::istream> inputStream, stride::Population* pop)
    : m_people(), m_commutes(), m_inputStream(std::move(inputStream)), m_population(pop)
{
}
void GeoGridReader::AddCommutes(GeoGrid& geoGrid)
{
        for (const auto& commute_tuple : m_commutes) {
                const auto a      = geoGrid.GetById(std::get<0>(commute_tuple));
                const auto b      = geoGrid.GetById(std::get<1>(commute_tuple));
                const auto amount = std::get<2>(commute_tuple);
                a->AddOutgoingCommute(b, amount);
                b->AddIncomingCommute(a, amount);
        }
}

} // namespace geopop
