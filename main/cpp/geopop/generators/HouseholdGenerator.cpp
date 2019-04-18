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

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template<>
void Generator<stride::ContactType::Id::Household>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig)
{
        vector<double> weights;
        for (const auto& loc : geoGrid) {
                weights.push_back(loc->GetPopFraction());
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        auto       pop  = geoGrid.GetPopulation();

        for (auto i = 0U; i < ggConfig.info.count_households; i++) {
                const auto loc = geoGrid[dist()];
                AddPools(*loc, pop, ggConfig);
        }
}

} // namespace geopop
