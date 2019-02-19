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

#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <spdlog/logger.h>

namespace stride {
class ContactPool;
}

namespace geopop {

class GeoGrid;
class GeoGridConfig;
class Location;

class College;
class Household;
class K12School;
class PrimaryCommunity;
class SecondaryCommunity;
class Workplace;

/**
 * Interface for populators. They generate some data and apply it to the GeoGrid.
 */
class Populator
{
public:
        /// Construct with a RnMan and a logger.
        explicit Populator(stride::util::RnMan&            rnManager,
                           std::shared_ptr<spdlog::logger> logger = stride::util::LogUtils::CreateNullLogger());

        /// Virtual destructor for inheritance.
        virtual ~Populator() = default;

        /// Populate the given geogrid for pool type (fixed in implementation).
        virtual void Apply(std::shared_ptr<GeoGrid> geogrid, const GeoGridConfig& geoGridConfig) = 0;

protected:
        /// Find contactpools in startRadius (in km) around start and, if none are found, double
        /// the radius and search again until the radius gets infinite. May return an empty vector
        /// when there are no pools to be found.
        template <typename T>
        std::vector<stride::ContactPool*> GetNearbyPools(const std::shared_ptr<GeoGrid>&  geoGrid,
                                                         const std::shared_ptr<Location>& start,
                                                         double                           startRadius = 10.0) const;

        /// Convenience wrapper around m_rnManager
        bool MakeChoice(double fraction);

protected:
        stride::util::RnMan&            m_rnManager; ///< RnManager used by populators.
        std::shared_ptr<spdlog::logger> m_logger;    ///< Logger used by populators.
};

extern template std::vector<stride::ContactPool*> Populator::GetNearbyPools<College>(
    const std::shared_ptr<GeoGrid>& geoGrid, const std::shared_ptr<Location>& start, double startRadius) const;

extern template std::vector<stride::ContactPool*> Populator::GetNearbyPools<Household>(
    const std::shared_ptr<GeoGrid>& geoGrid, const std::shared_ptr<Location>& start, double startRadius) const;

extern template std::vector<stride::ContactPool*> Populator::GetNearbyPools<K12School>(
    const std::shared_ptr<GeoGrid>& geoGrid, const std::shared_ptr<Location>& start, double startRadius) const;

extern template std::vector<stride::ContactPool*> Populator::GetNearbyPools<PrimaryCommunity>(
    const std::shared_ptr<GeoGrid>& geoGrid, const std::shared_ptr<Location>& start, double startRadius) const;

extern template std::vector<stride::ContactPool*> Populator::GetNearbyPools<SecondaryCommunity>(
    const std::shared_ptr<GeoGrid>& geoGrid, const std::shared_ptr<Location>& start, double startRadius) const;

extern template std::vector<stride::ContactPool*> Populator::GetNearbyPools<Workplace>(
    const std::shared_ptr<GeoGrid>& geoGrid, const std::shared_ptr<Location>& start, double startRadius) const;

} // namespace geopop
