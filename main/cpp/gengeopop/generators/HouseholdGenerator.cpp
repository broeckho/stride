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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "HouseholdGenerator.h"
#include "gengeopop/Household.h"

#include <trng/discrete_dist.hpp>

namespace gengeopop {

void HouseholdGenerator::Apply(std::shared_ptr<GeoGrid> geoGrid, const GeoGridConfig& geoGridConfig,
                               unsigned int& contactCenterCounter)
{
        std::vector<double> weights;
        for (const auto& loc : *geoGrid) {
                weights.push_back(loc->GetRelativePopulationSize());
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));

        for (auto i = 0U; i < geoGridConfig.popInfo.households; i++) {
                const auto loc = (*geoGrid)[dist()];
                const auto h   = std::make_shared<Household>(contactCenterCounter++);
                h->Fill(geoGrid);
                loc->AddContactCenter(h);
        }
}

} // namespace gengeopop
