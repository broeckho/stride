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

#include "Generator.h"

namespace gengeopop {

/**
 * Generate a GeoGrid out of generators for the various contact pool types.
 */

class GeoGridPoolBuilder
{
public:
        /// Construct with a config and a target GeoGrid.
        GeoGridPoolBuilder(GeoGridConfig& geoGridConfig, std::shared_ptr<GeoGrid> geoGrid);

        /// Add a generator for one of the contact pool types.
        void AddGenerator(std::shared_ptr<Generator> generator);

        /// Generate the geo grid.
        void BuildPools();

        /// Get the target GeoGrid.
        std::shared_ptr<GeoGrid> GetGeoGrid();

private:
        std::vector<std::shared_ptr<Generator>> m_generators;    ///< Generators
        std::shared_ptr<GeoGrid>                m_geoGrid;       ///< The target
        GeoGridConfig&                          m_geoGridConfig; ///< The config
};

} // namespace gengeopop
