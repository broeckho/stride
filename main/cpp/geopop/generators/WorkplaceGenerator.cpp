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

#include "Generator.h"

#include "geopop/GeoGridConfig.h"
#include "geopop/PoolParams.h"
#include "util/Assert.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template<>
void Generator<stride::ContactType::Id::Workplace>::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        // 1. active people count and the commuting people count are given
        // 2. count the workplaces
        // 3. count the working people at each location = #residents + #incoming commuters - #outgoing commuters
        // 4. use that information for the distribution
        // 5. assign each workplaces to a location

        const auto EmployeeCount = geoGridConfig.info.popcount_workplace;
        const auto WorkplacesCount =
            static_cast<unsigned int>(ceil(EmployeeCount / static_cast<double>(PoolParams<Id::Workplace>::people)));

        // = for each location #residents + #incoming commuting people - #outgoing commuting people
        vector<double> weights;
        for (const auto& loc : geoGrid) {
                const double ActivePeopleCount =
                    (loc->GetPopCount() +
                     loc->GetIncomingCommuteCount(geoGridConfig.param.fraction_workplace_commuters) -
                     loc->GetOutgoingCommuteCount(geoGridConfig.param.fraction_workplace_commuters) *
                         geoGridConfig.param.particpation_workplace);

                const double weight = ActivePeopleCount / EmployeeCount;
                AssertThrow(weight >= 0 && weight <= 1 && !std::isnan(weight), "Invalid weight: " + to_string(weight),
                            m_logger);
                weights.push_back(weight);
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        auto       pop  = geoGrid.GetPopulation();

        for (auto i = 0U; i < WorkplacesCount; i++) {
                const auto loc = geoGrid[dist()];
                AddPools(*loc, pop);
        }
}

} // namespace geopop
