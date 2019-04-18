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

#include "geopop/generators/HouseholdGenerator.h"

#include "../../createlogger.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/HouseholdCenter.h"
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

TEST(HouseholdGeneratorTest, OneLocationTest)
{
        RnMan              rnMan{RnInfo()}; // Default random number manager.
        HouseholdGenerator householdGenerator(rnMan, CreateTestLogger());
        GeoGridConfig      config{};

        ContactType::IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.popInfo.count_households         = 4;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        geoGrid.AddLocation(loc1);

        householdGenerator.Apply(geoGrid, config, contactCenterCounter);

        const auto& centersOfLoc1 = loc1->RefCenters(Id::Household);
        EXPECT_EQ(centersOfLoc1.size(), 4);
}

TEST(HouseholdGeneratorTest, ZeroLocationTest)
{
        RnMan              rnMan{RnInfo()}; // Default random number manager.
        HouseholdGenerator householdGenerator(rnMan, CreateTestLogger());
        GeoGridConfig      config{};

        ContactType::IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.popInfo.count_households         = 4;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        householdGenerator.Apply(geoGrid, config, contactCenterCounter);

        EXPECT_EQ(geoGrid.size(), 0);
}

TEST(HouseholdGeneratorTest, FiveLocationsTest)
{
        RnMan              rnMan{RnInfo()}; // Default random number manager.
        HouseholdGenerator householdGenerator(rnMan, CreateTestLogger());
        GeoGridConfig      config{};

        ContactType::IdSubscriptArray<unsigned int>   contactCenterCounter(1U);

        config.popInfo.count_households         = 4000;
        config.input.pop_size                   = 37542 * 100;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 10150 * 100);
        auto loc2    = make_shared<Location>(2, 4, Coordinate(0, 0), "Vlaams-Brabant", 10040 * 100);
        auto loc3    = make_shared<Location>(3, 4, Coordinate(0, 0), "Henegouwen", 7460 * 100);
        auto loc4    = make_shared<Location>(4, 4, Coordinate(0, 0), "Limburg", 3269 * 100);
        auto loc5    = make_shared<Location>(5, 4, Coordinate(0, 0), "Luxemburg", 4123 * 100);

        geoGrid.AddLocation(loc1);
        geoGrid.AddLocation(loc2);
        geoGrid.AddLocation(loc3);
        geoGrid.AddLocation(loc4);
        geoGrid.AddLocation(loc5);

        for (const shared_ptr<Location>& loc : geoGrid) {
                loc->SetRelativePop(static_cast<double>(loc->GetPopCount()) /
                                    static_cast<double>(config.input.pop_size));
        }

        householdGenerator.Apply(geoGrid, config, contactCenterCounter);

        const auto& centersOfLoc1 = loc1->RefCenters(Id::Household);
        EXPECT_EQ(centersOfLoc1.size(), 1179);

        const auto& centersOfLoc2 = loc2->RefCenters(Id::Household);
        EXPECT_EQ(centersOfLoc2.size(), 1137);

        const auto& centersOfLoc3 = loc3->RefCenters(Id::Household);
        EXPECT_EQ(centersOfLoc3.size(), 868);

        const auto& centersOfLoc4 = loc4->RefCenters(Id::Household);
        EXPECT_EQ(centersOfLoc4.size(), 358);

        const auto& centersOfLoc5 = loc5->RefCenters(Id::Household);
        EXPECT_EQ(centersOfLoc5.size(), 458);
}

} // namespace
