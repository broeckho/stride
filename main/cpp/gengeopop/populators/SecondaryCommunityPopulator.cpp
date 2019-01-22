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

#include "SecondaryCommunityPopulator.h"

#include "gengeopop/K12School.h"
#include "gengeopop/SecondaryCommunity.h"
#include "pop/Person.h"

#include <set>

namespace gengeopop {

void SecondaryCommunityPopulator::Apply(std::shared_ptr<GeoGrid> geoGrid, GeoGridConfig&)
{
        m_logger->info("Starting to populate Secondary Communities");
        std::set<stride::ContactPool*> found;

        // for every location
        for (const auto& loc : *geoGrid) {
                if (loc->GetPopulation() == 0) {
                        continue;
                }
                // 1. find all communities in an area of 10-k*10 km
                const auto& nearbyPools = GetContactPoolInIncreasingRadius<SecondaryCommunity>(geoGrid, loc);

                // 2. find all households in this location
                const auto& households = loc->GetContactCentersOfType<Household>();

                auto hh_per_comm        = households.size() / nearbyPools.size();
                auto remainder          = households.size() % nearbyPools.size();
                auto current_comm       = 0U;
                auto current_hh_in_comm = 0U;

                for (const auto& household : households) {
                        auto housePool = household->GetPools()[0];
                        if ((current_hh_in_comm == hh_per_comm && (!remainder || current_comm >= remainder)) ||
                            (current_hh_in_comm == hh_per_comm + 1 && (remainder && current_comm < remainder))) {
                                current_comm++;
                                current_hh_in_comm = 0;
                        }
                        current_hh_in_comm++;
                        auto pool = nearbyPools[current_comm];
                        for (auto p : *housePool) {
                                found.insert(pool);
                                pool->AddMember(p);
                                p->SetSecondaryCommunityId(static_cast<unsigned int>(pool->GetId()));
                        }
                }
        }

        m_logger->info("Finished populating Secondary Communities");
        m_logger->info("Used {} different Secondary communities", found.size());
}
} // namespace gengeopop
