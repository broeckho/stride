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

#include "PartialPopulator.h"

namespace gengeopop {

/**
 * Add population to a GeoGrid out of PartionPopulators (simliar to GeoGridGenerator).
 */
class GeoGridPopulator
{
public:
        /// Construct with a config and a target GeoGrid.
        GeoGridPopulator(GeoGridConfig& geoGridConfig, std::shared_ptr<GeoGrid> geoGrid);

        /// Add a PartialPopulator to use when populating.
        void AddPartialPopulator(std::shared_ptr<PartialPopulator> gen);

        /// Apply all partial populators onto an empty GeoGrid object.
        void PopulateGeoGrid();

        /// Get the target GeoGrid.
        std::shared_ptr<GeoGrid> GetGeoGrid();

private:
        std::vector<std::shared_ptr<PartialPopulator>> m_partialPopulators; ///< PartialPopulators.
        std::shared_ptr<GeoGrid>                       m_geoGrid;           ///< The target geogrid.
        GeoGridConfig                                  m_geoGridConfig;     ///< Configuration of target geogrid.
};

} // namespace gengeopop
