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

#include "K12SchoolGenerator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/K12SchoolCenter.h"
#include "geopop/Location.h"
#include "util/RnMan.h"

namespace geopop {

using namespace std;
using namespace stride::ContactType;

void K12SchoolGenerator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig,
                               IdSubscriptArray<unsigned int>& ccCounter)
{
        // 1. given the number of persons of school age, calculate number of schools; schools
        //    have 500 pupils on average
        // 2. assign schools to a location by using a discrete distribution which reflects the
        //    relative number of pupils for that location; the relative number of pupils is set
        //    to the relative population w.r.t the total population.

        const auto pupilCount = geoGridConfig.popInfo.popcount_k12school;
        const auto schoolCount =
            static_cast<unsigned int>(ceil(pupilCount / static_cast<double>(geoGridConfig.pools.k12school_size)));

        vector<double> weights;
        for (const auto& loc : geoGrid) {
                weights.push_back(loc->GetRelativePop());
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);

        for (auto i = 0U; i < schoolCount; i++) {
                const auto loc = geoGrid[dist()];
                const auto k12 = make_shared<K12SchoolCenter>(ccCounter[Id::K12School]++);
                k12->SetupPools(geoGridConfig, geoGrid.GetPopulation());
                loc->AddCenter(k12);
        }
}

} // namespace geopop
