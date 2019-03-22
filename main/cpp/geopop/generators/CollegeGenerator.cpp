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

#include "CollegeGenerator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/Assert.h"
#include "util/RnMan.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

void CollegeGenerator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        const auto studentCount = geoGridConfig.popInfo.popcount_college;
        const auto collegeCount =
            static_cast<unsigned int>(ceil(studentCount / static_cast<double>(geoGridConfig.pools.college_size)));
        const auto cities = geoGrid.TopK(10);

        if (cities.empty()) {
                // trng can't handle empty vectors
                return;
        }

        // Aggregate population in TopK cities.
        auto totalPop = 0U;
        for (const auto& c : cities) {
                totalPop += c->GetPopCount();
        }

        // Weights determined by relative population in city.
        vector<double> weights;
        for (const auto& c : cities) {
                const auto weight = static_cast<double>(c->GetPopCount()) / static_cast<double>(totalPop);
                AssertThrow(weight >= 0 && weight <= 1 && !std::isnan(weight),
                            "CollegeGenerator> Invalid weight: " + to_string(weight), m_logger);
                weights.push_back(weight);
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        auto       pop  = geoGrid.GetPopulation();

        for (auto i = 0U; i < collegeCount; i++) {
                auto loc = cities[dist()];
                AddPools(*loc, pop, geoGridConfig.pools.pools_per_college);
        }
}

void CollegeGenerator::AddPools(Location& loc, Population* pop, unsigned int number)
{
        auto& poolSys = pop->RefPoolSys();
        for (auto i = 0U; i < number; ++i) {
                const auto p = poolSys.CreateContactPool(stride::ContactType::Id::College);
                loc.RegisterPool<Id::College>(p);
        }
}

} // namespace geopop
