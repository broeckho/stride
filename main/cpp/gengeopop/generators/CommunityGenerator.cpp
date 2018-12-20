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

#include "CommunityGenerator.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/PrimaryCommunity.h"
#include "gengeopop/SecondaryCommunity.h"

#include <trng/discrete_dist.hpp>
#include <cmath>
#include <iostream>

namespace gengeopop {

void CommunityGenerator::Apply(std::shared_ptr<GeoGrid> geoGrid, GeoGridConfig& geoGridConfig)
{
        /*
         * 1. calculate amount of communities, each community has average 2000 persons
         * 2. assign communities to a location by using a discrete distribution which reflects the relative amount of
         * people for that location
         */

        int  populationSize = geoGridConfig.input.populationSize;
        auto amountOfCommunities =
            static_cast<int>(std::ceil(populationSize / geoGridConfig.constants.meanCommunitySize));

        std::vector<double> weights;

        for (const std::shared_ptr<Location>& loc : *geoGrid) {
                double weight =
                    static_cast<double>(loc->GetPopulation()) / static_cast<double>(geoGridConfig.input.populationSize);
                CheckWeight("CommunityGenerator", weight);
                weights.push_back(weight);
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));
        for (int communityId = 0; communityId < amountOfCommunities; communityId++) {
                int                               locationId = dist();
                std::shared_ptr<Location>         loc        = (*geoGrid)[locationId];
                std::shared_ptr<PrimaryCommunity> community =
                    std::make_shared<PrimaryCommunity>(geoGridConfig.generated.contactCenters++);
                community->Fill(geoGrid);
                loc->AddContactCenter(community);
        }
        for (int communityId = 0; communityId < amountOfCommunities; communityId++) {
                int                                 locationId = dist();
                std::shared_ptr<Location>           loc        = (*geoGrid)[locationId];
                std::shared_ptr<SecondaryCommunity> community =
                    std::make_shared<SecondaryCommunity>(geoGridConfig.generated.contactCenters++);
                community->Fill(geoGrid);
                loc->AddContactCenter(community);
        }
}

} // namespace
