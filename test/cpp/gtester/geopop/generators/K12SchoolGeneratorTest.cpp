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

#include "geopop/generators/K12SchoolGenerator.h"

#include "../../createlogger.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/K12SchoolCenter.h"
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

TEST(SchoolGeneratorTest, OneLocationTest)
{
        RnMan              rnMan{RnInfo()}; // Default random number manager.
        K12SchoolGenerator schoolGenerator(rnMan, CreateTestLogger());
        GeoGridConfig      config{};

        IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.input.pop_size                   = 10000;
        config.popInfo.popcount_k12school       = 2000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        geoGrid.AddLocation(loc1);

        schoolGenerator.Apply(geoGrid, config, contactCenterCounter);

        const auto& centersOfLoc1 = loc1->RefCenters(Id::K12School);
        EXPECT_EQ(centersOfLoc1.size(), 4);
}

TEST(SchoolGeneratorTest, ZeroLocationTest)
{
        RnMan              rnMan{RnInfo()}; // Default random number manager.
        K12SchoolGenerator schoolGenerator(rnMan, CreateTestLogger());
        GeoGridConfig      config{};

        IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.input.pop_size                   = 10000;
        config.popInfo.popcount_k12school       = 2000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        schoolGenerator.Apply(geoGrid, config, contactCenterCounter);

        EXPECT_EQ(geoGrid.size(), 0);
}

TEST(SchoolGeneratorTest, FiveLocationsTest)
{
        RnMan              rnMan{RnInfo()}; // Default random number manager.
        K12SchoolGenerator schoolGenerator(rnMan, CreateTestLogger());
        GeoGridConfig      config{};

        IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.input.pop_size                   = 37542 * 100;
        config.popInfo.popcount_k12school       = 750840;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 10150 * 100);
        auto loc2    = make_shared<Location>(1, 4, Coordinate(0, 0), "Vlaams-Brabant", 10040 * 100);
        auto loc3    = make_shared<Location>(1, 4, Coordinate(0, 0), "Henegouwen", 7460 * 100);
        auto loc4    = make_shared<Location>(1, 4, Coordinate(0, 0), "Limburg", 3269 * 100);
        auto loc5    = make_shared<Location>(1, 4, Coordinate(0, 0), "Luxemburg", 4123 * 100);

        geoGrid.AddLocation(loc1);
        geoGrid.AddLocation(loc2);
        geoGrid.AddLocation(loc3);
        geoGrid.AddLocation(loc4);
        geoGrid.AddLocation(loc5);

        for (const shared_ptr<Location>& loc : geoGrid) {
                loc->SetRelativePop(static_cast<double>(loc->GetPopCount()) /
                                    static_cast<double>(config.input.pop_size));
        }

        schoolGenerator.Apply(geoGrid, config, contactCenterCounter);

        const auto& centersOfLoc1 = loc1->RefCenters(Id::K12School);
        EXPECT_EQ(centersOfLoc1.size(), 444);

        const auto& centersOfLoc2 = loc2->RefCenters(Id::K12School);
        EXPECT_EQ(centersOfLoc2.size(), 416);

        const auto& centersOfLoc3 = loc3->RefCenters(Id::K12School);
        EXPECT_EQ(centersOfLoc3.size(), 330);

        const auto& centersOfLoc4 = loc4->RefCenters(Id::K12School);
        EXPECT_EQ(centersOfLoc4.size(), 133);

        const auto& centersOfLoc5 = loc5->RefCenters(Id::K12School);
        EXPECT_EQ(centersOfLoc5.size(), 179);
}

} // namespace
