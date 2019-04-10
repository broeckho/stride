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

#include "Generator.h"

#include "geopop/GeoGridConfig.h"
#include "geopop/PoolParams.h"
#include "util/Assert.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template<>
void Generator<stride::ContactType::Id::PrimaryCommunity>::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        // 1. calculate number of communities
        // 2. assign communities to a location using a discrete distribution reflecting
        //    the relative number of people at that location

        const auto popCount       = geoGridConfig.param.pop_size;
        const auto communitySize  = PoolParams<Id::PrimaryCommunity>::people;
        const auto communityCount = static_cast<unsigned int>(ceil(popCount / static_cast<double>(communitySize)));

        vector<double> weights;
        for (const auto& loc : geoGrid) {
                const auto weight = static_cast<double>(loc->GetPopCount()) / static_cast<double>(popCount);
                AssertThrow(weight >= 0 && weight <= 1 && !std::isnan(weight),
                            "CommunityGenerator> Invalid weight: " + to_string(weight), m_logger);
                weights.push_back(weight);
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        auto       pop  = geoGrid.GetPopulation();

        for (auto i = 0U; i < communityCount; i++) {
                const auto loc = geoGrid[dist()];
                AddPools(*loc, pop);
        }
}

} // namespace geopop
