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

#include "CollegePopulator.h"

#include "gengeopop/College.h"
#include "pop/Person.h"
#include "util/ExcAssert.h"

#include <trng/uniform_int_dist.hpp>

namespace gengeopop {

using namespace std;

void CollegePopulator::Apply(shared_ptr<GeoGrid> geoGrid, GeoGridConfig& geoGridConfig)
{
        m_logger->info("Starting to populate Colleges");

        set<stride::ContactPool*> found;
        unsigned int              students  = 0;
        unsigned int              commuting = 0;

        // for every location
        for (const shared_ptr<Location>& loc : *geoGrid) {
                if (loc->GetPopulation() == 0) {
                        continue;
                }
                // 1. find all highschools in an area of 10-k*10 km
                const vector<stride::ContactPool*>& nearByColleges =
                    GetContactPoolInIncreasingRadius<College>(geoGrid, loc);

                ExcAssert(!nearByColleges.empty(),
                          "Did not find any HighSchool due to invalid input data in CollegePopulator");

                auto distNonCommuting = m_rnManager[0].variate_generator(trng::uniform_int_dist(
                    0, static_cast<trng::uniform_int_dist::result_type>(nearByColleges.size())));

                // 2. find all highschools were students from this location commute to
                vector<Location*> commutingHighSchools;
                vector<double>    commutingWeights;
                for (const pair<Location*, double>& commute : loc->GetOutgoingCommuningCities()) {
                        const auto& highSchools = commute.first->GetContactCentersOfType<College>();
                        if (!highSchools.empty()) {
                                commutingHighSchools.push_back(commute.first);
                                commutingWeights.push_back(commute.second);
                        }
                }

                function<trng::discrete_dist::result_type()> disCommuting;

                if (!commutingWeights.empty()) {
                        disCommuting = m_rnManager[0].variate_generator(
                            trng::discrete_dist(commutingWeights.begin(), commutingWeights.end()));
                }

                // 2. for every student assign a class
                for (const shared_ptr<ContactCenter>& household : loc->GetContactCentersOfType<Household>()) {
                        stride::ContactPool* contactPool = household->GetPools()[0];
                        found.insert(contactPool);
                        for (stride::Person* p : *contactPool) {
                                if (p->IsCollegeStudentCandidate() &&
                                    MakeChoice(geoGridConfig.input.fraction_1826_years_WhichAreStudents)) {
                                        students++;
                                        // this person is a student
                                        if (!commutingHighSchools.empty() &&
                                            MakeChoice(geoGridConfig.input.fraction_student_commutingPeople)) {
                                                // this person is commuting
                                                commuting++;

                                                // id of the location this person is commuting to
                                                auto locationId = disCommuting();
                                                // create list of classes for each highschool at this location
                                                const auto& highSchools = commutingHighSchools[locationId]
                                                                              ->GetContactCentersOfType<College>();

                                                vector<stride::ContactPool*> contactPools;
                                                for (const auto& highSchool : highSchools) {
                                                        contactPools.insert(contactPools.end(), highSchool->begin(),
                                                                            highSchool->end());
                                                }

                                                auto disPools = m_rnManager[0].variate_generator(trng::uniform_int_dist(
                                                    0, static_cast<trng::uniform_int_dist::result_type>(
                                                           contactPools.size())));

                                                auto id = disPools();
                                                contactPools[id]->AddMember(p);
                                                p->SetCollegeId(contactPools[id]->GetId());
                                        } else {
                                                auto id = distNonCommuting();
                                                nearByColleges[id]->AddMember(p);
                                                p->SetCollegeId(
                                                    static_cast<unsigned int>(nearByColleges[id]->GetId()));
                                        }
                                }
                        }
                }
        }
        m_logger->info("Number of students in Colleges: {}", students);
        m_logger->info("Number of classes:  {}", found.size());
        m_logger->info("Number students that commute: ", commuting);
}

} // namespace gengeopop
