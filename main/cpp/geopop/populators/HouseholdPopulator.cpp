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

#include "HouseholdPopulator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"
#include "geopop/K12School.h"
#include "geopop/Location.h"

namespace geopop {

using namespace std;
using namespace stride::ContactType;

void HouseholdPopulator::Apply(shared_ptr<GeoGrid> geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->info("Starting to populate Households");

        auto person_id = 0U;
        auto household_dist =
            m_rn_man.GetUniformIntGenerator(0, static_cast<int>(geoGridConfig.refHH.households.size()), 0U);

        for (const shared_ptr<Location>& loc : *geoGrid) {
                const vector<shared_ptr<ContactCenter>>& households = loc->GetContactCentersOfType(Id::Household);
                for (const auto& h : households) {
                        auto contactPool = h->GetPools()[0];
                        auto hDraw       = static_cast<unsigned int>(household_dist());
                        auto hProfile    = geoGridConfig.refHH.households[hDraw]->GetPools()[0];
                        for (stride::Person* p : *hProfile) {
                                auto person = geoGrid->CreatePerson(person_id++, p->GetAge(), contactPool->GetId(), 0,
                                                                    0, 0, 0, 0);
                                contactPool->AddMember(person);
                        }
                }
        }
        m_logger->debug("Number of persons in households: {}", person_id);
}

} // namespace geopop
