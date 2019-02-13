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

#include "Populator.h"

#include "gengeopop/GeoGrid.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/Location.h"

#include "gengeopop/College.h"
#include "gengeopop/Household.h"
#include "gengeopop/K12School.h"
#include "gengeopop/PrimaryCommunity.h"
#include "gengeopop/SecondaryCommunity.h"
#include "gengeopop/Workplace.h"

#include "pool/ContactPool.h"

#include <trng/discrete_dist.hpp>

namespace gengeopop {

Populator::Populator(stride::util::RnMan& rnManager, std::shared_ptr<spdlog::logger> logger)
    : m_rnManager(rnManager), m_logger(std::move(logger))
{
}

bool Populator::MakeChoice(double fraction)
{
        std::vector<double> weights;
        weights.push_back(1.0 - fraction); // -> 0, return is false -> not part of the fraction
        weights.push_back(fraction);       // -> 1, return is true -> part of the fraction

        auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));
        return static_cast<bool>(dist());
}

template <typename T>
std::vector<stride::ContactPool*> Populator::GetPoolInIncreasingRadius(const std::shared_ptr<GeoGrid>&  geoGrid,
                                                            const std::shared_ptr<Location>& start,
                                                            double startRadius) const
{
        double                            currentRadius = startRadius;
        std::vector<stride::ContactPool*> pools;

        while (pools.empty()) {
                for (const std::shared_ptr<Location>& nearLoc :
                        geoGrid->LocationsInRadius(start, currentRadius)) {
                        const auto& centers = nearLoc->GetContactCentersOfType<T>();
                        for (const auto& center : centers) {
                                pools.insert(pools.end(), center->begin(), center->end());
                        }
                }
                currentRadius *= 2;
                if (currentRadius == std::numeric_limits<double>::infinity()) {
                        break;
                }
        }
        return pools;
}

template std::vector<stride::ContactPool*> Populator::GetPoolInIncreasingRadius<College>(
        const std::shared_ptr<GeoGrid>&  geoGrid,
        const std::shared_ptr<Location>& start,
        double startRadius) const;

template std::vector<stride::ContactPool*> Populator::GetPoolInIncreasingRadius<Household>(
        const std::shared_ptr<GeoGrid>&  geoGrid,
        const std::shared_ptr<Location>& start,
        double startRadius) const;

template std::vector<stride::ContactPool*> Populator::GetPoolInIncreasingRadius<K12School>(
        const std::shared_ptr<GeoGrid>&  geoGrid,
        const std::shared_ptr<Location>& start,
        double startRadius) const;

template std::vector<stride::ContactPool*> Populator::GetPoolInIncreasingRadius<PrimaryCommunity>(
        const std::shared_ptr<GeoGrid>&  geoGrid,
        const std::shared_ptr<Location>& start,
        double startRadius) const;

template std::vector<stride::ContactPool*> Populator::GetPoolInIncreasingRadius<SecondaryCommunity>(
        const std::shared_ptr<GeoGrid>&  geoGrid,
        const std::shared_ptr<Location>& start,
        double startRadius) const;

template std::vector<stride::ContactPool*> Populator::GetPoolInIncreasingRadius<Workplace>(
        const std::shared_ptr<GeoGrid>&  geoGrid,
        const std::shared_ptr<Location>& start,
        double startRadius) const;


} // namespace gengeopop
