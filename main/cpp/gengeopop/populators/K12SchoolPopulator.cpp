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

#include "K12SchoolPopulator.h"
#include "gengeopop/K12School.h"
#include <trng/discrete_dist.hpp>
#include <trng/lcg64.hpp>
#include <trng/uniform_int_dist.hpp>
#include <cmath>
#include <gengeopop/K12School.h>
#include <iostream>
#include <pop/Person.h>
#include <set>

namespace gengeopop {

void K12SchoolPopulator::Apply(std::shared_ptr<GeoGrid> geoGrid, GeoGridConfig&)
{
        m_logger->info("Starting to populate Schools");

        std::set<stride::ContactPool*> found;
        unsigned int                   pupils = 0;

        // for every location
        for (const std::shared_ptr<Location>& loc : *geoGrid) {
                if (loc->GetPopulation() == 0) {
                        continue;
                }

                // 1. find all schools in an area of 10-k*10 km
                const std::vector<stride::ContactPool*>& classes =
                    GetContactPoolInIncreasingRadius<K12School>(geoGrid, loc);

                auto dist = m_rnManager[0].variate_generator(
                    trng::uniform_int_dist(0, static_cast<trng::uniform_int_dist::result_type>(classes.size())));

                // 2. for every student assign a class
                for (const std::shared_ptr<ContactCenter>& household : loc->GetContactCentersOfType<Household>()) {
                        stride::ContactPool* contactPool = household->GetPools()[0];
                        found.insert(contactPool);
                        for (stride::Person* person : *contactPool) {
                                if (person->IsStudentCandidate()) {
                                        auto id = dist();
                                        classes[id]->AddMember(person);
                                        person->SetK12SchoolId(classes[id]->GetId());
                                        pupils++;
                                }
                        }
                }
        }

        m_logger->info("Finished populating Schools");
        m_logger->info("Used {} different classes", found.size());
        m_logger->info("Placed {} pupils in Schools", pupils);
}

} // namespace gengeopop
