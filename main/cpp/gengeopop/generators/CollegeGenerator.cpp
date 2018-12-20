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

#include "CollegeGenerator.h"
#include "gengeopop/College.h"
#include <trng/discrete_dist.hpp>
#include <trng/lcg64.hpp>
#include <cmath>
#include <gengeopop/College.h>
#include <iostream>
#include <queue>

namespace gengeopop {

void CollegeGenerator::Apply(std::shared_ptr<GeoGrid> geoGrid, GeoGridConfig& geoGridConfig)
{
        int  amountOfPupils  = geoGridConfig.calculated.popcount_1826_years_and_student;
        auto amountOfSchools = static_cast<int>(std::ceil(amountOfPupils / geoGridConfig.constants.meanCollegeSize));

        std::vector<std::shared_ptr<Location>> cities = geoGrid->TopK(10);

        if (cities.empty()) {
                // trng can't handle empty vectors
                return;
        }

        int totalCitiesPopulation = 0;

        for (const std::shared_ptr<Location>& loc : cities) {
                totalCitiesPopulation += loc->GetPopulation();
        }

        std::vector<double> weights;

        for (const std::shared_ptr<Location>& loc : cities) {
                double weight =
                    static_cast<double>(loc->GetPopulation()) / static_cast<double>((double)totalCitiesPopulation);

                CheckWeight("CollegeGenerator", weight);

                weights.push_back(weight);
        }

        auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));

        for (int schoolId = 0; schoolId < amountOfSchools; schoolId++) {
                int                       locationId = dist();
                std::shared_ptr<Location> loc        = cities[locationId];
                auto highschool = std::make_shared<College>(geoGridConfig.generated.contactCenters++);
                highschool->Fill(geoGrid);
                loc->AddContactCenter(highschool);
        }
}

} // namespace
