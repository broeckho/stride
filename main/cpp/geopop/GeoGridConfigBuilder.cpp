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
 *  Copyright 2019, Jan Broeckhove.
 */

#include "GeoGridConfigBuilder.h"

#include "GeoGridConfig.h"
#include "contact/AgeBrackets.h"
#include "geopop/io/ReaderFactory.h"

#include <cmath>

namespace geopop {

using namespace std;
using namespace stride::AgeBrackets;

void GeoGridConfigBuilder::SetData(GeoGridConfig& geoGridConfig, const string& householdsFileName)
{
        ReaderFactory readerFactory;

        auto householdsReader = readerFactory.CreateHouseholdReader(householdsFileName);
        householdsReader->SetReferenceHouseholds(geoGridConfig.refHH.households, geoGridConfig.refHH.persons,
                                                 geoGridConfig.refHH.pools);
        const auto popSize = geoGridConfig.input.pop_size;

        //----------------------------------------------------------------
        // Determine age makeup of reference houshold population.
        //----------------------------------------------------------------
        const auto ref_hh_count  = geoGridConfig.refHH.households.size();
        const auto ref_pop_count = geoGridConfig.refHH.persons.size();
        const auto averageHhSize = static_cast<double>(ref_pop_count) / ref_hh_count;

        auto ref_k12school_age = 0U;
        auto ref_college_age   = 0U;
        auto ref_workplace_age = 0U;
        for (const auto& p : geoGridConfig.refHH.persons) {
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
        const auto fraction_k12school_age = static_cast<double>(ref_k12school_age) / static_cast<double>(ref_pop_count);
        const auto fraction_college_age   = static_cast<double>(ref_college_age) / static_cast<double>(ref_pop_count);
        const auto fraction_workplace_age = static_cast<double>(ref_workplace_age) / static_cast<double>(ref_pop_count);

        const auto age_count_k12school = static_cast<unsigned int>(floor(popSize * fraction_k12school_age));
        const auto age_count_college   = static_cast<unsigned int>(floor(popSize * fraction_college_age));
        const auto age_count_workplace = static_cast<unsigned int>(floor(popSize * fraction_workplace_age));

        geoGridConfig.popInfo.popcount_k12school = age_count_k12school;

        geoGridConfig.popInfo.popcount_college =
            static_cast<unsigned int>(floor(geoGridConfig.input.participation_college * age_count_college));

        geoGridConfig.popInfo.popcount_workplace =
            static_cast<unsigned int>(floor(geoGridConfig.input.particpation_workplace *
                                            (age_count_workplace - geoGridConfig.popInfo.popcount_college)));

        geoGridConfig.popInfo.count_households =
            static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));
}

} // namespace geopop
