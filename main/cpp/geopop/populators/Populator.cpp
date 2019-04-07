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

#include "geopop/College.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"
#include "geopop/K12School.h"
#include "geopop/Location.h"
#include "geopop/PrimaryCommunity.h"
#include "geopop/SecondaryCommunity.h"
#include "geopop/Workplace.h"

#include "contact/ContactPool.h"

#include <trng/discrete_dist.hpp>

namespace geopop {

using namespace std;
using namespace stride;

Populator::Populator(util::RnMan& rnManager, shared_ptr<spdlog::logger> logger)
    : m_rnManager(rnManager), m_logger(move(logger))
{
}

bool Populator::MakeChoice(double fraction)
{
        vector<double> weights;
        weights.push_back(1.0 - fraction); // -> 0, return is false -> not part of the fraction
        weights.push_back(fraction);       // -> 1, return is true -> part of the fraction

        auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));
        return static_cast<bool>(dist());
}

template <typename T>
vector<ContactPool*> Populator::GetNearbyPools(const shared_ptr<GeoGrid>& geoGrid, const shared_ptr<Location>& start,
                                               double startRadius) const
{
        double               currentRadius = startRadius;
        vector<ContactPool*> pools;

        while (pools.empty()) {
                for (const shared_ptr<Location>& nearLoc : geoGrid->LocationsInRadius(start, currentRadius)) {
                        const auto& centers = nearLoc->GetContactCentersOfType<T>();
                        for (const auto& center : centers) {
                                pools.insert(pools.end(), center->begin(), center->end());
                        }
                }
                currentRadius *= 2;
                if (currentRadius == numeric_limits<double>::infinity()) {
                        break;
                }
        }
        return pools;
}

template vector<ContactPool*> Populator::GetNearbyPools<College>(const shared_ptr<GeoGrid>&  geoGrid,
                                                                 const shared_ptr<Location>& start,
                                                                 double                      startRadius) const;

template vector<ContactPool*> Populator::GetNearbyPools<Household>(const shared_ptr<GeoGrid>&  geoGrid,
                                                                   const shared_ptr<Location>& start,
                                                                   double                      startRadius) const;

template vector<ContactPool*> Populator::GetNearbyPools<K12School>(const shared_ptr<GeoGrid>&  geoGrid,
                                                                   const shared_ptr<Location>& start,
                                                                   double                      startRadius) const;

template vector<ContactPool*> Populator::GetNearbyPools<PrimaryCommunity>(const shared_ptr<GeoGrid>&  geoGrid,
                                                                          const shared_ptr<Location>& start,
                                                                          double startRadius) const;

template vector<ContactPool*> Populator::GetNearbyPools<SecondaryCommunity>(const shared_ptr<GeoGrid>&  geoGrid,
                                                                            const shared_ptr<Location>& start,
                                                                            double startRadius) const;

template vector<ContactPool*> Populator::GetNearbyPools<Workplace>(const shared_ptr<GeoGrid>&  geoGrid,
                                                                   const shared_ptr<Location>& start,
                                                                   double                      startRadius) const;

} // namespace geopop
