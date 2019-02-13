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

#include "CommunityGenerator.h"

#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/Location.h"
#include "gengeopop/PrimaryCommunity.h"
#include "gengeopop/SecondaryCommunity.h"

#include <trng/discrete_dist.hpp>
#include <cmath>
#include <iostream>

namespace gengeopop {

using namespace std;

void CommunityGenerator::Apply(shared_ptr<GeoGrid> geoGrid, const GeoGridConfig& geoGridConfig,
                               unsigned int& contactCenterCounter)
{
        // 1. calculate number of communities, each community has average 2000 persons
        // 2. assign communities to a location using a discrete distribution reflecting the relative number of
        //    people at that location

        const auto popCount = geoGridConfig.input.pop_size;
        const auto communityCount =
            static_cast<unsigned int>(ceil(popCount / geoGridConfig.constants.mean_community_size));

        vector<double> weights;
        for (const auto& loc : *geoGrid) {
                const auto weight = static_cast<double>(loc->GetPopCount()) / static_cast<double>(popCount);
                CheckWeight("CommunityGenerator", weight);
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
                pc->Fill(geoGrid);
                loc->AddContactCenter(pc);
        }
        for (auto i = 0U; i < communityCount; i++) {
                const auto loc = (*geoGrid)[dist()];
                const auto sc  = make_shared<SecondaryCommunity>(contactCenterCounter++);
                sc->Fill(geoGrid);
                loc->AddContactCenter(sc);
        }
}

} // namespace gengeopop
