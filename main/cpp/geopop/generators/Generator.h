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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "util/RnMan.h"

#include <spdlog/logger.h>

namespace stride {
class Population;
namespace util {
class RnMan;
}
} // namespace stride

namespace geopop {

class GeoGrid;
class GeoGridConfig;
class Location;

/**
 * An interface base class for generators that provide geo data and apply it onto the GeoGrid.
 */
class Generator
{
public:
        /// Constructor with random number manager and logger.
        explicit Generator(stride::util::RnMan rnMan, std::shared_ptr<spdlog::logger> logger = nullptr);

        /// Virtual destructor for inheritance
        virtual ~Generator() = default;

        /// Generate ContactPools as sepcified by the data in GeoGridConfig.
        virtual void Apply(GeoGrid& geogrid, const GeoGridConfig& geoGridConfig) = 0;

        /// Create a given number ContactPools in the GeoGrid.
        virtual void AddPools(Location& loc, stride::Population* pop, unsigned int number) = 0;

protected:
        stride::util::RnMan             m_rn_man; ///< RnManager used by generators.
        std::shared_ptr<spdlog::logger> m_logger; ///< Logger used by generators.
};

} // namespace geopop
