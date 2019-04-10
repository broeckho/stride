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

#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "geopop/Location.h"
#include "pop/Person.h"

#include <set>

using namespace std;
using namespace stride;
using namespace stride::ContactType;

namespace geopop {

template<>
void Populator<stride::ContactType::Id::SecondaryCommunity>::Apply(GeoGrid& geoGrid, const GeoGridConfig&)
{
        m_logger->trace("Starting to populate Secondary Communities");

        for (const auto& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                // 1. find all communities in an area of 10-k*10 km
                const auto& nearbyPools = geoGrid.GetNearbyPools(Id::SecondaryCommunity, *loc);

                if (nearbyPools.empty()) { // apparently no commumity pools present, so nothing to populate
                        return;
                }

                // 2. find all households in this location
                const auto& households = loc->RefPools(Id::Household);

                auto hh_per_comm        = households.size() / nearbyPools.size();
                auto remainder          = households.size() % nearbyPools.size();
                auto current_comm       = 0U;
                auto current_hh_in_comm = 0U;

                for (const auto& housePool : households) {
                        if ((current_hh_in_comm == hh_per_comm && (!remainder || current_comm >= remainder)) ||
                            (current_hh_in_comm == hh_per_comm + 1 && (remainder && current_comm < remainder))) {
                                current_comm++;
                                current_hh_in_comm = 0;
                        }
                        current_hh_in_comm++;
                        auto pool = nearbyPools[current_comm];
                        for (auto p : *housePool) {
                                pool->AddMember(p);
                                p->SetPoolId(Id::SecondaryCommunity, pool->GetId());
                        }
                }
        }

        m_logger->trace("Done populating Secondary Communities");
}
} // namespace geopop
