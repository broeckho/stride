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

#include "../../createlogger.h"
#include "geopop/CollegeCenter.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "geopop/generators/K12SchoolGenerator.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

TEST(CollegeGeneratorTest, OneLocationTest)
{
        RnMan            rnMan{RnInfo()}; // Default random number manager.
        CollegeGenerator collegeGenerator(rnMan, CreateTestLogger());
        GeoGridConfig    config{};

        IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.input.pop_size                 = 45000;
        config.popInfo.popcount_college       = 9000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", config.input.pop_size);

        geoGrid.AddLocation(loc1);

        collegeGenerator.Apply(geoGrid, config, contactCenterCounter);

        const auto& centersOfLoc1 = loc1->RefCenters(Id::College);
        EXPECT_EQ(centersOfLoc1.size(), 3);
}

TEST(CollegeGeneratorTest, ZeroLocationTest)
{
        RnMan            rnMan{RnInfo()}; // Default random number manager.
        CollegeGenerator collegeGenerator(rnMan, CreateTestLogger());
        GeoGridConfig    config{};

        IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.input.pop_size                 = 10000;
        config.popInfo.popcount_college       = 2000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        collegeGenerator.Apply(geoGrid, config, contactCenterCounter);

        EXPECT_EQ(geoGrid.size(), 0);
}

TEST(CollegeGeneratorTest, FiveLocationsTest)
{
        RnMan            rnMan{RnInfo()}; // Default random number manager.
        CollegeGenerator collegeGenerator(rnMan, CreateTestLogger());
        GeoGridConfig    config{};

        IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.input.pop_size                 = 399992;
        config.popInfo.popcount_college       = 79998;

        auto        pop     = Population::Create();
        auto        geoGrid = GeoGrid(pop.get());
        vector<int> sizes{28559, 33319, 39323, 37755, 35050, 10060, 13468, 8384,
                          9033,  31426, 33860, 4110,  50412, 25098, 40135};
        for (int size : sizes) {
                const auto loc = make_shared<Location>(1, 4, Coordinate(0, 0), "Size: " + to_string(size), size);
                geoGrid.AddLocation(loc);
        }
        collegeGenerator.Apply(geoGrid, config, contactCenterCounter);

        vector<int> expectedCount{2, 2, 5, 2, 3, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3};
        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expectedCount[i], geoGrid[i]->RefCenters(Id::College).size());
        }
}

} // namespace
