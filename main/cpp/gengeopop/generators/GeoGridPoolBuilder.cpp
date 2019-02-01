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

#include "GeoGridPoolBuilder.h"

#include <iostream>
#include <memory>
#include <utility>

namespace gengeopop {

GeoGridPoolBuilder::GeoGridPoolBuilder(GeoGridConfig& geoGridConfig, std::shared_ptr<GeoGrid> geoGrid)
    : m_generators(), m_geoGrid(std::move(geoGrid)), m_geoGridConfig(geoGridConfig)
{
}

void GeoGridPoolBuilder::BuildPools()
{
        for (const auto& g : m_generators) {
                g->Apply(m_geoGrid, m_geoGridConfig);
        }
}

std::shared_ptr<GeoGrid> GeoGridPoolBuilder::GetGeoGrid() { return m_geoGrid; }

void GeoGridPoolBuilder::AddGenerator(std::shared_ptr<Generator> generator) { m_generators.push_back(generator); }

} // namespace gengeopop
