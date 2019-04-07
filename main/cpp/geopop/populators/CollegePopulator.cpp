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
#include "geopop/College.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"
#include "geopop/Location.h"
#include "pop/Person.h"
#include "util/Assert.h"

#include <trng/discrete_dist.hpp>
#include <trng/uniform_int_dist.hpp>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

void CollegePopulator::Apply(shared_ptr<GeoGrid> geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->info("Starting to populate Colleges");

        set<ContactPool*> found;
        auto              students  = 0U;
        auto              commuting = 0U;

        // for every location
        for (const auto& loc : *geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                // 1. find all highschools in an area of 10-k*10 km
                const auto& nearByColleges = GetNearbyPools<College>(geoGrid, loc);

                AssertThrow(!nearByColleges.empty(), "No HighSchool found!", m_logger);

                const auto distNonCommuting = m_rnManager[0].variate_generator(
                    trng::uniform_int_dist(0, static_cast<trng::uniform_int_dist::result_type>(nearByColleges.size())));

                // 2. find all colleges where students from this location commute to
                vector<Location*> commutingCollege;
                vector<double>    commutingWeights;
                for (const auto& commute : loc->GetOutgoingCommutingCities()) {
                        const auto& highSchools = commute.first->GetContactCentersOfType<College>();
                        if (!highSchools.empty()) {
                                commutingCollege.push_back(commute.first);
                                commutingWeights.push_back(commute.second);
                        }
                }

                function<trng::discrete_dist::result_type()> disCommuting;

                if (!commutingWeights.empty()) {
                        disCommuting = m_rnManager[0].variate_generator(
                            trng::discrete_dist(commutingWeights.begin(), commutingWeights.end()));
                }

                // 2. for every student assign a class
                for (const auto& household : loc->GetContactCentersOfType<Household>()) {
                        ContactPool* contactPool = household->GetPools()[0];
                        found.insert(contactPool);
                        for (Person* p : *contactPool) {
                                if (AgeBrackets::College::HasAge(p->GetAge()) &&
                                    MakeChoice(geoGridConfig.input.participation_college)) {
                                        students++;
                                        // this person is a student
                                        if (!commutingCollege.empty() &&
                                            MakeChoice(geoGridConfig.input.fraction_college_commuters)) {
                                                // this person is commuting
                                                commuting++;

                                                // id of the location this person is commuting to
                                                auto locationId = disCommuting();
                                                // create list of classes for each highschool at this location
                                                const auto& highSchools =
                                                    commutingCollege[locationId]->GetContactCentersOfType<College>();

                                                vector<ContactPool*> contactPools;
                                                for (const auto& hs : highSchools) {
                                                        contactPools.insert(contactPools.end(), hs->begin(), hs->end());
                                                }

                                                auto disPools = m_rnManager[0].variate_generator(trng::uniform_int_dist(
                                                    0, static_cast<trng::uniform_int_dist::result_type>(
                                                           contactPools.size())));

                                                auto id = disPools();
                                                contactPools[id]->AddMember(p);
                                                p->SetPoolId(Id::College, contactPools[id]->GetId());
                                        } else {
                                                auto id = distNonCommuting();
                                                nearByColleges[id]->AddMember(p);
                                                p->SetPoolId(Id::College, nearByColleges[id]->GetId());
                                        }
                                }
                        }
                }
        }
        m_logger->info("Number of students in Colleges: {}", students);
        m_logger->info("Number of classes:  {}", found.size());
        m_logger->info("Number students that commute: ", commuting);
}

} // namespace geopop
