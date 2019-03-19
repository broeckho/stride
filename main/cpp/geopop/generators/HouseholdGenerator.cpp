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

#include "HouseholdGenerator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/Location.h"
#include "util/RnMan.h"

using namespace std;
using namespace stride::ContactType;

namespace geopop {

void HouseholdGenerator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig,
                               IdSubscriptArray<unsigned int>& ccCounter)
{
        vector<double> weights;
        for (const auto& loc : geoGrid) {
                weights.push_back(loc->GetRelativePop());
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);

        for (auto i = 0U; i < geoGridConfig.popInfo.count_households; i++) {
                const auto loc = geoGrid[dist()];
                const auto h   = std::make_shared<HouseholdCenter>(ccCounter[Id::Household]++);
                h->SetupPools(geoGridConfig, geoGrid.GetPopulation());
                loc->AddCenter(h);
        }
}

} // namespace geopop
