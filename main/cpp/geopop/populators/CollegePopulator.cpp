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

#include "CollegePopulator.h"

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

void CollegePopulator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->trace("Starting to populate Colleges");

        // for every location
        for (const auto& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                // 1. find all highschools in an area of 10-k*10 km
                const auto& nearByCollegePools = GetNearbyPools(Id::College, geoGrid, *loc);

                AssertThrow(!nearByCollegePools.empty(), "No College found!", m_logger);

                const auto distNonCommuting =
                    m_rn_man.GetUniformIntGenerator(0, static_cast<int>(nearByCollegePools.size()), 0U);

                // 2. find all colleges where students from this location commute to
                vector<Location*> commutingCollege;
                vector<double>    commutingWeights;
                for (const auto& commute : loc->CRefOutgoingCommutes()) {
                        const auto& cpools = commute.first->CRefPools(Id::College);
                        if (!cpools.empty()) {
                                commutingCollege.push_back(commute.first);
                                commutingWeights.push_back(commute.second);
                        }
                }

                function<int()> disCommuting;

                if (!commutingWeights.empty()) {
                        disCommuting = m_rn_man.GetDiscreteGenerator(commutingWeights, 0U);
                }

                // 2. for every student assign a class
                for (const auto& hhPool : loc->RefPools(Id::Household)) {
                        for (Person* p : *hhPool) {
                                if (AgeBrackets::College::HasAge(p->GetAge()) &&
                                    MakeChoice(geoGridConfig.input.participation_college)) {
                                        // this person is a student
                                        if (!commutingCollege.empty() &&
                                            MakeChoice(geoGridConfig.input.fraction_college_commuters)) {
                                                // this person is commuting

                                                // pools to commute to
                                                const auto& collegePools =
                                                    commutingCollege[disCommuting()]->CRefPools(Id::College);

                                                auto disPools = m_rn_man.GetUniformIntGenerator(
                                                    0, static_cast<int>(collegePools.size()), 0U);
                                                auto idraw = disPools();
                                                collegePools[idraw]->AddMember(p);
                                                p->SetPoolId(Id::College, collegePools[idraw]->GetId());
                                        } else {
                                                auto idraw = distNonCommuting();
                                                nearByCollegePools[idraw]->AddMember(p);
                                                p->SetPoolId(Id::College, nearByCollegePools[idraw]->GetId());
                                        }
                                }
                        }
                }
        }

        m_logger->trace("Done populating Colleges");
}

} // namespace geopop
