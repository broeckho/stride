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

#include "K12SchoolGenerator.h"

#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/K12School.h"

#include <trng/discrete_dist.hpp>

namespace gengeopop {

using namespace std;

void K12SchoolGenerator::Apply(shared_ptr<GeoGrid> geoGrid, GeoGridConfig& geoGridConfig)
{
        // 1. given the number of persons of school age, calculate number of schools; schools
        //    have 500 pupils on average
        // 2. assign schools to a location by using a discrete distribution which reflects the
        //    relative number of pupils for that location; the relative number of pupils is set
        //    to the relative population w.r.t the total population.

        int  pupilCount  = geoGridConfig.calculated.compulsoryPupils;
        auto schoolCount = static_cast<int>(ceil(pupilCount / geoGridConfig.constants.meanK12SchoolSize));

        vector<double> weights;
        for (const shared_ptr<Location>& loc : *geoGrid) {
                weights.push_back(loc->GetRelativePopulationSize());
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));

        for (int i = 0; i < schoolCount; i++) {
                auto loc = (*geoGrid)[dist()];
                auto k12 = make_shared<K12School>(geoGridConfig.generated.contactCenters++);
                k12->Fill(geoGrid);
                loc->AddContactCenter(k12);
        }
}

} // namespace gengeopop
