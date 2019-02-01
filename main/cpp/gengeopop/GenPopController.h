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

#include "GeoGridConfig.h"
#include "util/RnMan.h"

#include <spdlog/logger.h>

namespace gengeopop {

class CitiesReader;
class CommutesReader;

/**
 * Controls the complete generation and population of a GeoGrid.
 */
class GenPopController
{
public:
        /// Create a GenGeoPopController.
        GenPopController(std::shared_ptr<spdlog::logger> logger, GeoGridConfig& geoGridConfig,
                         stride::util::RnMan& rnManager, std::string citiesFileName, std::string commutingFileName,
                         std::string householdFileName);

        /// Reads the data files
        void ReadDataFiles();

        /// Build and store the Geo part of the GeoGrid.
        void GenGeo();

        /// Build and store the Pop part of the GeoGrid.
        void GenPop();

        /// Get the generated GeoGrid.
        std::shared_ptr<GeoGrid> GetGeoGrid();

        /// Use a specified population storage during generation.
        void UsePopulation(std::shared_ptr<stride::Population> pop);

private:
        GeoGridConfig& m_geoGridConfig; ///< The GeoGridConfig used to generate the grid.

        stride::util::RnMan& m_rnManager; ///< The random number generation manager.

        std::shared_ptr<GeoGrid> m_geoGrid; ///< The generated GeoGrid.

        std::shared_ptr<stride::Population> m_population; ///< The generated GeoGrid.

        std::shared_ptr<CitiesReader>    m_citiesReader;     ///< The CitiesReader.
        std::shared_ptr<CommutesReader>  m_commutesReader;   ///< The CommutesReader.
        std::shared_ptr<HouseholdReader> m_householdsReader; ///< The HouseholdsReader.
        std::shared_ptr<spdlog::logger>  m_logger;           ///< The logger

        std::string m_citiesFileName;     ///< Filename for info on cities.
        std::string m_commutingFileName;  ///< Filename for info on commutes.
        std::string m_householdsFileName; ///< Filename for info on households.
};

} // namespace gengeopop
