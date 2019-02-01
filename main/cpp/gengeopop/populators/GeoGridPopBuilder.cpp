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

#include "GeoGridPopBuilder.h"

#include <iostream>
#include <memory>

namespace gengeopop {

GeoGridPopBuilder::GeoGridPopBuilder(GeoGridConfig& geoGridConfig, std::shared_ptr<GeoGrid> geoGrid)
    : m_populators(), m_geoGrid(std::move(geoGrid)), m_geoGridConfig(geoGridConfig)
{
}

void GeoGridPopBuilder::BuildPop()
{
        for (std::shared_ptr<Populator>& partialGen : m_populators) {
                partialGen->Apply(m_geoGrid, m_geoGridConfig);
        }
}

std::shared_ptr<GeoGrid> GeoGridPopBuilder::GetGeoGrid() { return m_geoGrid; }

void GeoGridPopBuilder::AddPartialPopulator(std::shared_ptr<Populator> gen) { m_populators.push_back(gen); }

} // namespace gengeopop
