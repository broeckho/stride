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
 *  Copyright 2019, Jan Broeckhove and Bistromatics group.
 */

#include "GeoGridConfigBuilder.h"

#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/io/ReaderFactory.h"
#include "pool/AgeBrackets.h"

#include <cmath>

namespace gengeopop {

using namespace std;
using namespace stride::AgeBrackets;

void GeoGridConfigBuilder::SetData(GeoGridConfig& geoGridConfig, const string& householdsFileName)
{
        ReaderFactory readerFactory;

        auto householdsReader = readerFactory.CreateHouseholdReader(householdsFileName);
        householdsReader->SetReferenceHouseholds(geoGridConfig.popInfo.reference_households,
                                                 geoGridConfig.popInfo.persons, geoGridConfig.popInfo.contact_pools);
        const auto popSize       = geoGridConfig.input.pop_size;


        //----------------------------------------------------------------
        // Determine age makeup of reference houshold population.
        //----------------------------------------------------------------
        const auto ref_hh_count  = geoGridConfig.popInfo.reference_households.size();
        const auto ref_pop_count = geoGridConfig.popInfo.persons.size();
        const auto averageHhSize = static_cast<double>(ref_pop_count) / ref_hh_count;

        auto ref_k12school_age = 0U;
        auto ref_college_age = 0U;
        auto ref_workplace_age = 0U;
        for (const auto& p : geoGridConfig.popInfo.persons) {
        const auto age = p.GetAge();
                if (K12School::HasAge(age)) {
                        ref_k12school_age++;
                }
                if (College::HasAge(age)) {
                        ref_college_age++;
                }
                if (Workplace::HasAge(age)) {
                        ref_workplace_age++;
                }
        }

        //----------------------------------------------------------------
        // Scale up to the generated population size.
        //----------------------------------------------------------------
        const auto fraction_k12school_age = static_cast<double>(ref_k12school_age)/static_cast<double>(ref_pop_count);
        const auto fraction_college_age   = static_cast<double>(ref_college_age)/static_cast<double>(ref_pop_count);
        const auto fraction_workplace_age = static_cast<double>(ref_workplace_age)/static_cast<double>(ref_pop_count);

        const auto age_count_k12school = static_cast<unsigned int>(floor(popSize * fraction_k12school_age));
        const auto age_count_college   = static_cast<unsigned int>(floor(popSize * fraction_college_age));
        const auto age_count_workplace = static_cast<unsigned int>(floor(popSize * fraction_workplace_age));

        geoGridConfig.popInfo.age_count_k12school = age_count_k12school;

        geoGridConfig.popInfo.popcount_1826_student = static_cast<unsigned int>(
            floor(geoGridConfig.input.fraction_1826_student * age_count_college));

        geoGridConfig.popInfo.popcount_1865_active = static_cast<unsigned int>(
            floor(geoGridConfig.input.fraction_1865_active *
                  (age_count_workplace - geoGridConfig.popInfo.popcount_1826_student)));

        geoGridConfig.popInfo.households =
            static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));
}

} // namespace gengeopop
