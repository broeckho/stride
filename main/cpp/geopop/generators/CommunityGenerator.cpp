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

#include "CommunityGenerator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "geopop/PrimaryCommunity.h"
#include "geopop/SecondaryCommunity.h"
#include "util/Assert.h"
#include "util/RnMan.h"

#include <trng/discrete_dist.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace geopop {

using namespace std;

void CommunityGenerator::Apply(shared_ptr<GeoGrid> geoGrid, const GeoGridConfig& geoGridConfig,
                               unsigned int& contactCenterCounter)
{
        // 1. calculate number of communities, each community has average 2000 persons
        // 2. assign communities to a location using a discrete distribution reflecting the relative number of
        //    people at that location

        const auto popCount = geoGridConfig.input.pop_size;
        const auto communityCount =
            static_cast<unsigned int>(ceil(popCount / static_cast<double>(geoGridConfig.pools.community_size)));

        vector<double> weights;
        for (const auto& loc : *geoGrid) {
                const auto weight = static_cast<double>(loc->GetPopCount()) / static_cast<double>(popCount);
                AssertThrow(weight >= 0 && weight <= 1 && !std::isnan(weight),
                            "CommunityGenerator> Invalid weight: " + to_string(weight), m_logger);
                weights.push_back(weight);
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));

        for (auto i = 0U; i < communityCount; i++) {
                const auto loc = (*geoGrid)[dist()];
                const auto pc  = make_shared<PrimaryCommunity>(contactCenterCounter++);
                pc->Fill(geoGridConfig, geoGrid);
                loc->AddContactCenter(pc);
        }
        for (auto i = 0U; i < communityCount; i++) {
                const auto loc = (*geoGrid)[dist()];
                const auto sc  = make_shared<SecondaryCommunity>(contactCenterCounter++);
                sc->Fill(geoGridConfig, geoGrid);
                loc->AddContactCenter(sc);
        }
}

} // namespace geopop
