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

#include <geopop/GeoGridConfig.h>
#include "PreSchoolPopulator.h"

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

    void PreSchoolPopulator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
    {
        m_logger->trace("Starting to populate preschools");

        for (const auto& loc : geoGrid) {
            if (loc->GetPopCount() == 0) {
                continue;
            }

            // 1. find all preschools in an area of 10-k*10 km
            const vector<ContactPool*>& preschools = GetNearbyPools(Id::PreSchool, geoGrid, *loc);

            auto dist = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(preschools.size()), 0U);

            // 2. for every kid assign a preschool
            for (auto& pool : loc->RefPools(Id::Household)) {
                for (Person* p : *pool) {
                    if (AgeBrackets::Daycare::HasAge(p->GetAge()) &&
                        MakeChoice(geoGridConfig.input.participation_preschool)) {
                        auto& c = preschools[dist()];
                        c->AddMember(p);
                        p->SetPoolId(Id::PreSchool, c->GetId());
                    }
                }
            }
        }

        m_logger->trace("Done populating Preschools");
    }

} // namespace geopop
