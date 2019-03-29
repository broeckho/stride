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

#include "DaycarePopulator.h"

#include "contact/AgeBrackets.h"
#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Person.h"
#include "util/Assert.h"

namespace geopop {

    using namespace std;
    using namespace stride;
    using namespace stride::ContactType;

    void DaycarePopulator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
    {
        m_logger->trace("Starting to populate Daycares");

        for (const auto& loc : geoGrid) {
            if (loc->GetPopCount() == 0) {
                continue;
            }

            // 1. find all daycares in an area of 10-k*10 km
            const vector<ContactPool*>& daycares = GetNearbyPools(Id::Daycare, geoGrid, *loc);

            auto dist = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(daycares.size()), 0U);

            // 2. for every toddler assign a daycare
            for (auto& pool : loc->RefPools(Id::Household)) {
                for (Person* p : *pool) {
                    if (AgeBrackets::Daycare::HasAge(p->GetAge()) &&
                        MakeChoice(geoGridConfig.input.participation_daycare)) {
                        auto& c = daycares[dist()];
                        c->AddMember(p);
                        p->SetPoolId(Id::Daycare, c->GetId());
                    }
                }
            }
        }

        m_logger->trace("Done populating Daycares");
    }

} // namespace geopop