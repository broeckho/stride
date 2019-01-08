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

#include "SecondaryCommunityPopulator.h"

#include "gengeopop/K12School.h"
#include "gengeopop/SecondaryCommunity.h"
#include "pop/Person.h"

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

                auto households_per_community        = households.size() / nearbyPools.size();
                auto remainder                       = households.size() % nearbyPools.size();
                auto current_community               = 0U;
                auto current_households_in_community = 0U;

                for (unsigned int i = 0; i < households.size(); i++) {
                        auto housePool = households[i]->GetPools()[0];
                        if ((current_households_in_community == households_per_community &&
                             (!remainder || current_community >= remainder)) ||
                            (current_households_in_community == households_per_community + 1 &&
                             (remainder && current_community < remainder))) {
                                current_community++;
                                current_households_in_community = 0;
                        }
                        current_households_in_community++;
                        auto pool = nearbyPools[current_community];
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
