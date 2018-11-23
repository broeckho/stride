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

#pragma once

#include "GeoGridConfig.h"
#include <gengeopop/io/CommutesReader.h>
#include <spdlog/logger.h>
#include <util/RnMan.h>

namespace gengeopop {

/**
 * Controls the complete generation and population of a GeoGrid.
 */
class GenGeoPopController
{
public:
        /// Create a GenGeoPopController
        GenGeoPopController(std::shared_ptr<spdlog::logger> logger, GeoGridConfig& geoGridConfig,
                            stride::util::RnMan& rnManager, std::string citiesFileName, std::string commutingFileName,
                            std::string householdFileName);

        /// Reads the data files
        void ReadDataFiles();

        /// Generate and store the Geo part of the GeoGrid
        void GenGeo();

        /// Populate and store the Pop part of the GeoGrid
        void GenPop();

        /// Get the generated GeoGrid
        std::shared_ptr<GeoGrid> GetGeoGrid();

        /// Use the given Population storage during generation
        void UsePopulation(std::shared_ptr<stride::Population> pop);

private:
        GeoGridConfig& m_geoGridConfig; ///< The GeoGridConfig used to generate

        stride::util::RnMan& m_rnManager; ///< The RnMan used

        std::shared_ptr<GeoGrid> m_geoGrid; ///< The generated GeoGrid

        std::shared_ptr<stride::Population> m_population; ///< The generated GeoGrid

        std::shared_ptr<CitiesReader>    m_citiesReader;     ///< The CitiesReader
        std::shared_ptr<CommutesReader>  m_commutesReader;   ///< The CommutesReader
        std::shared_ptr<HouseholdReader> m_householdsReader; ///< The HouseholdsReader
        std::shared_ptr<spdlog::logger>  m_logger;           ///< The logger used

        std::string m_citiesFileName;     ///< Filename of the file which stores information about the cities
        std::string m_commutingFileName;  ///< Filename of the file which stores information about the commutes
        std::string m_householdsFileName; ///< Filename of the file which stores information about the households
};

} // namespace gengeopop
