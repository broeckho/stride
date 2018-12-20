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

#include "WorkplaceGenerator.h"

#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/K12School.h"
#include "gengeopop/Workplace.h"

#include <trng/discrete_dist.hpp>

namespace gengeopop {

void WorkplaceGenerator::Apply(std::shared_ptr<GeoGrid> geoGrid, GeoGridConfig& geoGridConfig)
{
        /*
         * 1. active people count and the commuting people count are given
         * 2. count the workplaces, each workplace has an average of 20 employees
         * 3. count the working people at each location = #residents + #incoming commuters - #outgoing commuters
         * 4. use the last information for the distribution
         * 5. assign each workplace to a location
         */

        auto EmployeeCount = geoGridConfig.calculated.popcount_1865_and_years_active;
        auto WorkplacesCount =
            static_cast<int>(std::ceil(EmployeeCount / geoGridConfig.constants.meanWorkplaceSchoolSize));

        // = for each location #residents + #incoming commuting people - #outgoing commuting people
        std::vector<double> weights;

        for (const std::shared_ptr<Location>& loc : *geoGrid) {
                double ActivePeopleCount =
                    (loc->GetPopulation() +
                     loc->IncomingCommutingPeople(geoGridConfig.input.fraction_active_commutingPeople) -
                     loc->OutGoingCommutingPeople(geoGridConfig.input.fraction_active_commutingPeople) *
                         geoGridConfig.input.fraction_1865_years_active);

                double weight = ActivePeopleCount / EmployeeCount;
                CheckWeight("WorkplaceGenerator", weight);
                weights.push_back(weight);
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));

        for (int workPlaceId = 0; workPlaceId < WorkplacesCount; workPlaceId++) {
                int                       locationId = dist();
                std::shared_ptr<Location> loc        = (*geoGrid)[locationId];
                auto workplace = std::make_shared<Workplace>(geoGridConfig.generated.contactCenters++);
                workplace->Fill(geoGrid);
                loc->AddContactCenter(workplace);
        }
}

} // namespace gengeopop
