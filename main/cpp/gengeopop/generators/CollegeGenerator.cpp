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

#include "CollegeGenerator.h"

#include "gengeopop/College.h"

#include <trng/discrete_dist.hpp>

namespace gengeopop {

using namespace std;

void CollegeGenerator::Apply(shared_ptr<GeoGrid> geoGrid, GeoGridConfig& geoGridConfig)
{
        int  pupilCount  = geoGridConfig.calculated.popcount_1826_years_and_student;
        auto schoolCount = static_cast<int>(ceil(pupilCount / geoGridConfig.constants.meanCollegeSize));

        vector<shared_ptr<Location>> cities = geoGrid->TopK(10);

        if (cities.empty()) {
                // trng can't handle empty vectors
                return;
        }

        // Aggregate population in cities.
        int totalPop = 0;
        for (const shared_ptr<Location>& c : cities) {
                totalPop += c->GetPopulation();
        }

        // Weights determined by relative population in city.
        vector<double> weights;
        for (const shared_ptr<Location>& c : cities) {
                double weight = static_cast<double>(c->GetPopulation()) / static_cast<double>(totalPop);
                CheckWeight("CollegeGenerator", weight);
                weights.push_back(weight);
        }

        auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));

        for (int i = 0; i < schoolCount; i++) {
                auto loc     = cities[dist()];
                auto college = make_shared<College>(geoGridConfig.generated.contactCenters++);
                college->Fill(geoGrid);
                loc->AddContactCenter(college);
        }
}

} // namespace gengeopop
