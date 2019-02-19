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

#include "PrimaryCommunityPopulator.h"

#include "geopop/GeoGrid.h"
#include "geopop/Household.h"
#include "geopop/Location.h"
#include "geopop/PrimaryCommunity.h"

#include <trng/uniform_int_dist.hpp>

namespace geopop {

void PrimaryCommunityPopulator::Apply(std::shared_ptr<GeoGrid> geoGrid, const GeoGridConfig&)
{
        m_logger->info("Starting to populate Primary Communities");

        std::set<stride::ContactPool*> found;
        // for every location
        for (const std::shared_ptr<Location>& loc : *geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }

                // 1. find all communities in an area of 10-k*10 km
                const auto& nearbyPools = GetNearbyPools<PrimaryCommunity>(geoGrid, loc);

                // 2. for every household assign a community
                const auto dist = m_rnManager[0].variate_generator(
                    trng::uniform_int_dist(0, static_cast<trng::uniform_int_dist::result_type>(nearbyPools.size())));

                for (const auto& household : loc->GetContactCentersOfType<Household>()) {
                        auto contactPool = household->GetPools()[0];
                        for (auto p : *contactPool) {
                                auto& pool = nearbyPools[dist()];
                                found.insert(pool);
                                pool->AddMember(p);
                                p->SetPoolId(stride::ContactType::Id::PrimaryCommunity, pool->GetId());
                        }
                }
        }

        m_logger->info("Finished populating Primary Communities");
        m_logger->info("Used {} different Primary communities", found.size());
}

} // namespace geopop
