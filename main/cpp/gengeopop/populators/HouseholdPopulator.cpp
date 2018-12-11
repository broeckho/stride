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

#include "HouseholdPopulator.h"

#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/Household.h"
#include "gengeopop/K12School.h"

#include <trng/uniform_int_dist.hpp>

namespace gengeopop {

using namespace std;

void HouseholdPopulator::Apply(shared_ptr<GeoGrid> geoGrid, GeoGridConfig& geoGridConfig)
{
        m_logger->info("Starting to populate Households");

        unsigned int current_person_id = 0;
        auto         household_dist    = m_rnManager[0].variate_generator(trng::uniform_int_dist(
            0, static_cast<trng::uniform_int_dist::result_type>(geoGridConfig.generated.household_types.size())));

        for (const shared_ptr<Location>& loc : *geoGrid) {
                const vector<shared_ptr<ContactCenter>>& households = loc->GetContactCentersOfType<Household>();
                for (const auto& household : households) {
                        stride::ContactPool* contactPool     = household->GetPools()[0];
                        auto                 householdTypeId = static_cast<unsigned int>(household_dist());
                        stride::ContactPool* householdType =
                            geoGridConfig.generated.household_types[householdTypeId]->GetPools()[0];
                        for (stride::Person* personType : *householdType) {
                                auto person = geoGrid->CreatePerson(current_person_id++, personType->GetAge(),
                                                                    contactPool->GetId(), 0, 0, 0, 0, 0);
                                contactPool->AddMember(person);
                        }
                }
        }
        m_logger->info("Number of persons in households: {}", current_person_id);
}

} // namespace gengeopop
