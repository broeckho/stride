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

#include "WorkplaceGenerator.h"

#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/Assert.h"
#include "util/RnMan.h"

namespace geopop {

using namespace std;
using namespace stride::ContactType;

void WorkplaceGenerator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig, unsigned int& ccCounter)
{
        // 1. active people count and the commuting people count are given
        // 2. count the workplaces, each workplace has an average of 20 employees
        // 3. count the working people at each location = #residents + #incoming commuters - #outgoing commuters
        // 4. use the last information for the distribution
        // 5. assign each workplace to a location

        const auto EmployeeCount = geoGridConfig.popInfo.popcount_workplace;
        const auto WorkplacesCount =
            static_cast<unsigned int>(ceil(EmployeeCount / static_cast<double>(geoGridConfig.pools.workplace_size)));

        // = for each location #residents + #incoming commuting people - #outgoing commuting people
        vector<double> weights;
        for (const auto& loc : geoGrid) {
                const double ActivePeopleCount =
                    (loc->GetPopCount() +
                     loc->GetIncomingCommuteCount(geoGridConfig.input.fraction_workplace_commuters) -
                     loc->GetOutgoingCommuteCount(geoGridConfig.input.fraction_workplace_commuters) *
                         geoGridConfig.input.particpation_workplace);

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
        auto& poolSys = geoGrid.GetPopulation()->RefPoolSys();

        for (auto i = 0U; i < WorkplacesCount; i++) {
                const auto loc = geoGrid[dist()];
                const auto w   = make_shared<ContactCenter>(ccCounter++, Id::Workplace);

                // TODO CheckThisAlgorithm
                // for (std::size_t j = 0; j < geoGridConfig.pools.pools_per_workplace; ++j) {
                const auto p = poolSys.CreateContactPool(stride::ContactType::Id::Workplace);
                w->RegisterPool(p);
                loc->RegisterPool<Id::Workplace>(p);
                //}

                loc->AddCenter(w);
        }
}

void WorkplaceGenerator::SetupPools(Location& loc, ContactCenter& center, const GeoGridConfig&, stride::Population* pop)
{
        auto& poolSys = pop->RefPoolSys();

        // TODO CheckThisAlgorithm
        // for (std::size_t i = 0; i < geoGridConfig.pools.pools_per_workplace; ++i) {
        const auto p = poolSys.CreateContactPool(stride::ContactType::Id::Workplace);
        center.RegisterPool(p);
        loc.RegisterPool<Id::Workplace>(p);
        //}
}

} // namespace geopop
