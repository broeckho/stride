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

#include "GeoGridGenerator.h"
#include <iostream>
#include <memory>
#include <utility>
namespace gengeopop {

GeoGridGenerator::GeoGridGenerator(GeoGridConfig& geoGridConfig, std::shared_ptr<GeoGrid> geoGrid)
    : m_partialGenerators(), m_geoGrid(std::move(geoGrid)), m_geoGridConfig(geoGridConfig)
{
}

void GeoGridGenerator::GenerateGeoGrid()
{
        for (std::shared_ptr<PartialGenerator>& partialGen : m_partialGenerators) {
                partialGen->Apply(m_geoGrid, m_geoGridConfig);
        }
}

std::shared_ptr<GeoGrid> GeoGridGenerator::GetGeoGrid() { return m_geoGrid; }

void GeoGridGenerator::AddPartialGenerator(std::shared_ptr<PartialGenerator> gen)
{
        m_partialGenerators.push_back(gen);
}

} // namespace gengeopop
