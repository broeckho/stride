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

#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/Location.h"
#include "geopop/PrimaryCommunityCenter.h"
#include "pop/Person.h"

using namespace std;
using namespace stride::ContactType;

namespace geopop {

void PrimaryCommunityPopulator::Apply(GeoGrid& geoGrid, const GeoGridConfig&)
{
        m_logger->trace("Starting to populate Primary Communities");

        set<stride::ContactPool*> found;
        // for every location
        for (const shared_ptr<Location>& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }

                // 1. find all communities in an area of 10-k*10 km
                const auto& nearbyPools = GetNearbyPools(Id::PrimaryCommunity, geoGrid, *loc);

                // 2. for every household assign a community
                const auto dist = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(nearbyPools.size()), 0U);

                for (const auto& hhCenter : loc->RefCenters(Id::Household)) {
                        auto contactPool = (*hhCenter)[0];
                        for (auto p : *contactPool) {
                                auto& pool = nearbyPools[dist()];
                                found.insert(pool);
                                pool->AddMember(p);
                                p->SetPoolId(stride::ContactType::Id::PrimaryCommunity, pool->GetId());
                        }
                }
        }

        m_logger->debug("Finished populating Primary Communities");
        m_logger->debug("Used {} different Primary communities", found.size());
        m_logger->trace("Done populating Primary Communities");
}

} // namespace geopop
