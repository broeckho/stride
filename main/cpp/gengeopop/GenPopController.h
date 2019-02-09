#pragma once
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

#include "pop/Population.h"
#include "util/RnMan.h"

#include <spdlog/logger.h>

namespace gengeopop {

class GeoGrid;
class GeoGridConfig;

/**
 * Controls the complete generation and population of a GeoGrid.
 */
class GenPopController
{
public:
        /// Create a GenGeoPopController.
        GenPopController(std::shared_ptr<spdlog::logger> logger, stride::util::RnMan& rnManager,
                         std::shared_ptr<stride::Population> pop = stride::Population::Create());

        /// Reads the data files.
        void ReadDataFiles(GeoGridConfig& geoGridConfig, const std::string& citiesFileName,
                           const std::string& commutingFileName);

        /// Build and store the Geo part of the GeoGrid.
        void GenGeo(GeoGridConfig& geoGridConfig);

        /// Build and store the Pop part of the GeoGrid.
        void GenPop(GeoGridConfig& geoGridConfig);

        /// Get the generated GeoGrid.
        std::shared_ptr<GeoGrid> GetGeoGrid();

private:
        stride::util::RnMan&                m_rnManager;     ///< The random number generation manager.
        std::shared_ptr<GeoGrid>            m_geoGrid;       ///< The generated GeoGrid.
        std::shared_ptr<stride::Population> m_population;    ///< The generated GeoGrid.
        std::shared_ptr<spdlog::logger>     m_logger;        ///< The logger.
};

} // namespace gengeopop
