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

#include "SecondaryCommunityPopulator.h"

#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/Location.h"
#include "geopop/SecondaryCommunityCenter.h"
#include "pop/Person.h"

#include <set>

using namespace std;
using namespace stride;
using namespace stride::ContactType;

namespace geopop {

void SecondaryCommunityPopulator::Apply(GeoGrid& geoGrid, const GeoGridConfig&)
{
        m_logger->trace("Starting to populate Secondary Communities");

        set<stride::ContactPool*> found;

        // for every location
        for (const auto& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                // 1. find all communities in an area of 10-k*10 km
                const auto& nearbyPools = GetNearbyPools(Id::SecondaryCommunity, geoGrid, *loc);

                // 2. find all households in this location
                const auto& households = loc->RefCenters(Id::Household);

                auto hh_per_comm        = households.size() / nearbyPools.size();
                auto remainder          = households.size() % nearbyPools.size();
                auto current_comm       = 0U;
                auto current_hh_in_comm = 0U;

                for (const auto& hhCenter : households) {
                        auto housePool = (*hhCenter)[0];
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
                                p->SetPoolId(stride::ContactType::Id::SecondaryCommunity, pool->GetId());
                        }
                }
        }

        m_logger->debug("Used {} different Secondary communities", found.size());
        m_logger->trace("Done populating Secondary Communities");
}
} // namespace geopop
