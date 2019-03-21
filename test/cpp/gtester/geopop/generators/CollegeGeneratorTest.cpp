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

#include "geopop/generators/CollegeGenerator.h"

#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

// Checks whther generator can handle a single location.
TEST(CollegeGeneratorTest, OneLocationTest)
{
        RnMan            rnMan{RnInfo()};
        CollegeGenerator collegeGenerator(rnMan);
        unsigned int     ccCounter{1U};
        GeoGridConfig    config{};
        config.input.pop_size           = 45000;
        config.popInfo.popcount_college = 9000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", config.input.pop_size);

        geoGrid.AddLocation(loc1);
        collegeGenerator.Apply(geoGrid, config, ccCounter);

        const auto& centersOfLoc1 = loc1->CRefCenters(Id::College);
        EXPECT_EQ(centersOfLoc1.size(), 3);

        const auto& poolsOfLoc1 = loc1->CRefPools<Id::College>();
        EXPECT_EQ(poolsOfLoc1.size(), 3 * config.pools.pools_per_college);
}

// Checks whether Generator can handle zero locations in GeoGrid.
TEST(CollegeGeneratorTest, ZeroLocationTest)
{
        RnMan            rnMan{RnInfo()};
        CollegeGenerator collegeGenerator(rnMan);
        unsigned int     ccCounter{1U};
        GeoGridConfig    config{};
        config.input.pop_size           = 10000;
        config.popInfo.popcount_college = 2000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        collegeGenerator.Apply(geoGrid, config, ccCounter);

        EXPECT_EQ(geoGrid.size(), 0);
}

// Checks whether generator can handle multiple locations.
TEST(CollegeGeneratorTest, MultipleLocationsTest)
{
        RnMan            rnMan{RnInfo()};
        CollegeGenerator collegeGenerator(rnMan);
        unsigned int     ccCounter{1U};
        GeoGridConfig    config{};
        config.input.pop_size           = 399992;
        config.popInfo.popcount_college = 79998;

        auto        pop     = Population::Create();
        auto        geoGrid = GeoGrid(pop.get());

        vector<int> sizes{28559, 33319, 39323, 37755, 35050, 10060, 13468, 8384,
                          9033,  31426, 33860, 4110,  50412, 25098, 40135};
        for (int size : sizes) {
                const auto loc = make_shared<Location>(1, 4, Coordinate(0, 0), "Size: " + to_string(size), size);
                geoGrid.AddLocation(loc);
        }
        collegeGenerator.Apply(geoGrid, config, ccCounter);

        vector<int> expectedCount{2, 2, 5, 2, 3, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3};
        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expectedCount[i], geoGrid[i]->CRefCenters(Id::College).size());
                EXPECT_EQ(expectedCount[i] * config.pools.pools_per_college,
                                          geoGrid[i]->CRefPools<Id::College>().size());
        }


}

} // namespace
