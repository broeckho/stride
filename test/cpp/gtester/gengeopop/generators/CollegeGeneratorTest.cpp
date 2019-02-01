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

#include "gengeopop/generators/CollegeGenerator.h"
#include "../../createlogger.h"
#include "gengeopop/College.h"
#include "gengeopop/generators/K12SchoolGenerator.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace gengeopop;
using namespace stride;
using namespace stride::util;

namespace {

TEST(CollegeGeneratorTest, OneLocationTest)
{
        RnMan            rnManager{}; // Default random number manager.
        CollegeGenerator collegeGenerator(rnManager, CreateTestLogger());
        GeoGridConfig    config{};
        config.input.populationSize                       = 45000;
        config.calculated.popcount_1826_years_and_student = 9000;

        auto pop     = Population::Create();
        auto geoGrid = make_shared<GeoGrid>(pop.get());
        auto loc1    = make_shared<Location>(1, 4, 45000, Coordinate(0, 0), "Antwerpen");

        geoGrid->AddLocation(loc1);

        collegeGenerator.Apply(geoGrid, config);

        const auto& centersOfLoc1 = loc1->GetContactCenters();
        EXPECT_EQ(centersOfLoc1.size(), 3);
}

TEST(CollegeGeneratorTest, ZeroLocationTest)
{
        RnMan            rnManager{}; // Default random number manager.
        CollegeGenerator collegeGenerator(rnManager, CreateTestLogger());
        GeoGridConfig    config{};
        config.input.populationSize                       = 10000;
        config.calculated.popcount_1826_years_and_student = 2000;

        auto pop     = Population::Create();
        auto geoGrid = make_shared<GeoGrid>(pop.get());
        collegeGenerator.Apply(geoGrid, config);

        EXPECT_EQ(geoGrid->size(), 0);
}

TEST(CollegeGeneratorTest, FiveLocationsTest)
{
        RnMan            rnManager{}; // Default random number manager.
        CollegeGenerator collegeGenerator(rnManager, CreateTestLogger());
        GeoGridConfig    config{};
        config.input.populationSize                       = 399992;
        config.calculated.popcount_1826_years_and_student = 79998;

        auto        pop     = Population::Create();
        auto        geoGrid = make_shared<GeoGrid>(pop.get());
        vector<int> sizes{28559, 33319, 39323, 37755, 35050, 10060, 13468, 8384,
                          9033,  31426, 33860, 4110,  50412, 25098, 40135};
        for (int size : sizes) {
                geoGrid->AddLocation(make_shared<Location>(1, 4, size, Coordinate(0, 0), "Size: " + to_string(size)));
        }
        collegeGenerator.Apply(geoGrid, config);

        vector<int> expectedSchoolCount{2, 2, 5, 2, 3, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3};
        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expectedSchoolCount[i], geoGrid->Get(i)->GetContactCenters().size());
        }
}

} // namespace
