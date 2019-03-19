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

#include "geopop/populators/HouseholdPopulator.h"

#include "geopop/Coordinate.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>
#include <map>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

class HouseholdPopulatorTest : public testing::Test
{
public:
        HouseholdPopulatorTest() : householdPopulator(), rnManager(), config() {}

protected:
        void SetUp() override
        {
                rnManager   = make_shared<RnMan>(RnInfo{});
                auto logger = LogUtils::CreateCliLogger("stride_logger", "stride_log.txt");
                logger->set_level(spdlog::level::off);
                householdPopulator = make_shared<HouseholdPopulator>(*rnManager.get(), logger);
        }

        shared_ptr<HouseholdPopulator> householdPopulator;
        shared_ptr<RnMan>              rnManager;
        GeoGridConfig                  config;
};

TEST_F(HouseholdPopulatorTest, OneHouseholdTest)
{
        config.refHH.ages = vector<vector<unsigned int>>{{8U}};

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        auto hCenter = make_shared<HouseholdCenter>();
        hCenter->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(hCenter);
        geoGrid.AddLocation(loc1);

        householdPopulator->Apply(geoGrid, config);

        ASSERT_EQ(hCenter->size(), 1);
        EXPECT_EQ((*hCenter)[0]->size(), 1);
}

TEST_F(HouseholdPopulatorTest, ZeroHouseholdsTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());

        EXPECT_NO_THROW(householdPopulator->Apply(geoGrid, config));
}

TEST_F(HouseholdPopulatorTest, FiveHouseholdsTest)
{
        config.refHH.ages = vector<vector<unsigned int>>{{18U}};

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);

        auto household1 = make_shared<HouseholdCenter>();
        household1->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household1);
        auto household2 = make_shared<HouseholdCenter>();
        household2->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household2);
        auto household3 = make_shared<HouseholdCenter>();
        household3->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household3);
        auto household4 = make_shared<HouseholdCenter>();
        household4->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household4);
        auto household5 = make_shared<HouseholdCenter>();
        household5->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household5);

        geoGrid.AddLocation(loc1);

        householdPopulator->Apply(geoGrid, config);

        for (const auto& hCenter : loc1->RefCenters(Id::Household)) {
                ASSERT_EQ(hCenter->size(), 1);
                ASSERT_EQ((*hCenter)[0]->size(), 1);
                EXPECT_EQ((*(*hCenter)[0]->begin())->GetAge(), 18);
        }
}

TEST_F(HouseholdPopulatorTest, MultipleHouseholdTypesTest)
{
        config.refHH.ages = vector<vector<unsigned int>>{{18U}, {12U, 56U}};

        auto       pop     = Population::Create();
        auto       geoGrid = GeoGrid(pop.get());
        const auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        const auto hCenter = make_shared<HouseholdCenter>();

        hCenter->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(hCenter);
        geoGrid.AddLocation(loc1);

        const auto hCenter2 = make_shared<HouseholdCenter>();
        hCenter2->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(hCenter2);
        householdPopulator->Apply(geoGrid, config);
        
        {
                ASSERT_EQ(hCenter->size(), 1);
                EXPECT_EQ((*hCenter)[0]->size(), 1);
                EXPECT_EQ((*(*hCenter)[0]->begin())->GetAge(), 18);
        }
        {
                ASSERT_EQ(hCenter2->size(), 1);
                EXPECT_EQ((*hCenter2)[0]->size(), 2);
                EXPECT_EQ((*(*hCenter2)[0]->begin())->GetAge(), 12);
                EXPECT_EQ((*(*hCenter2)[0]->begin() + 1)->GetAge(), 56);
        }
}
