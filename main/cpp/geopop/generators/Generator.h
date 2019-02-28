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

#include <spdlog/logger.h>

namespace stride {
namespace util {
class RnMan;
}
} // namespace stride

namespace geopop {

class GeoGrid;
class GeoGridConfig;

/**
 * An interface base class for generators that provide geo data and apply it onto the GeoGrid.
 */
class Generator
{
public:
        /// Constructor with random number manager and logger.
        explicit Generator(stride::util::RnMan& rn_manager, std::shared_ptr<spdlog::logger> logger)
            : m_rnManager(rn_manager), m_logger(std::move(logger))
        {
        }

        /// Generate the contact centers for a pool type (fixed in implementation) to the geogrid.
        virtual void Apply(std::shared_ptr<GeoGrid> geogrid, const GeoGridConfig& geoGridConfig,
                           unsigned int& contactCenterCounter) = 0;

        /// Virtual destructor for inheritance
        virtual ~Generator() = default;

protected:
        stride::util::RnMan&            m_rnManager; ///< RnManager used by generators.
        std::shared_ptr<spdlog::logger> m_logger;    ///< Logger used by generators.
};

} // namespace geopop
