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
#include "geopop/Household.h"
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

        auto pop       = Population::Create();
        auto geoGrid   = make_shared<GeoGrid>(pop.get());
        auto loc1      = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        auto household = make_shared<Household>();
        household->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household);
        geoGrid->AddLocation(loc1);

        householdPopulator->Apply(geoGrid, config);

        const auto& pools = household->CRefPools();
        ASSERT_EQ(pools.size(), 1);
        EXPECT_EQ(pools[0]->GetPool().size(), 1);
}

TEST_F(HouseholdPopulatorTest, ZeroHouseholdsTest)
{
        auto pop     = Population::Create();
        auto geoGrid = make_shared<GeoGrid>(pop.get());

        EXPECT_NO_THROW(householdPopulator->Apply(geoGrid, config));
}

TEST_F(HouseholdPopulatorTest, FiveHouseholdsTest)
{
        config.refHH.ages = vector<vector<unsigned int>>{{18U}};

        auto pop     = Population::Create();
        auto geoGrid = make_shared<GeoGrid>(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);

        auto household1 = make_shared<Household>();
        household1->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household1);
        auto household2 = make_shared<Household>();
        household2->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household2);
        auto household3 = make_shared<Household>();
        household3->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household3);
        auto household4 = make_shared<Household>();
        household4->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household4);
        auto household5 = make_shared<Household>();
        household5->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household5);

        geoGrid->AddLocation(loc1);

        householdPopulator->Apply(geoGrid, config);

        for (const auto& household : loc1->RefCenters(Id::Household)) {
                ASSERT_EQ(household->CRefPools().size(), 1);
                ASSERT_EQ(household->CRefPools()[0]->GetPool().size(), 1);
                EXPECT_EQ((*household->CRefPools()[0]->begin())->GetAge(), 18);
        }
}

TEST_F(HouseholdPopulatorTest, MultipleHouseholdTypesTest)
{
        config.refHH.ages = vector<vector<unsigned int>>{{18U}, {12U, 56U}};

        auto       pop       = Population::Create();
        const auto geoGrid   = make_shared<GeoGrid>(pop.get());
        const auto loc1      = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        const auto household = make_shared<Household>();
        household->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household);
        geoGrid->AddLocation(loc1);
        auto household2 = make_shared<Household>();
        household2->RegisterPool(new ContactPool(0, ContactType::Id::Household));
        loc1->AddCenter(household2);
        householdPopulator->Apply(geoGrid, config);

        map<int, vector<ContactPool*>> pools_map;
        pools_map[household->CRefPools()[0]->GetPool().size()]  = household->CRefPools();
        pools_map[household2->CRefPools()[0]->GetPool().size()] = household2->CRefPools();
        {
                const auto& pools = pools_map[1];
                ASSERT_EQ(pools.size(), 1);
                EXPECT_EQ(pools[0]->GetPool().size(), 1);
                EXPECT_EQ((*pools[0]->begin())->GetAge(), 18);
        }
        {
                const auto& pools = pools_map[2];
                ASSERT_EQ(pools.size(), 1);
                EXPECT_EQ(pools[0]->GetPool().size(), 2);
                EXPECT_EQ((*pools[0]->begin())->GetAge(), 12);
                EXPECT_EQ((*(pools[0]->begin() + 1))->GetAge(), 56);
        }
}
