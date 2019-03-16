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

#include "geopop/populators/PrimaryCommunityPopulator.h"
#include "geopop/populators/SecondaryCommunityPopulator.h"

#include "geopop/Coordinate.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"
#include "geopop/Location.h"
#include "geopop/PrimaryCommunity.h"
#include "geopop/SecondaryCommunity.h"
#include "pop/Population.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

template <typename CommunityType>
class CommunityPopulatorTest : public testing::Test
{
public:
        CommunityPopulatorTest()
            : populator(), rnManager(), config(), location(), community(), geoGrid(), person(), logger()
        {
        }

protected:
        void SetUp() override
        {
                rnManager = make_shared<RnMan>(RnInfo{});
                logger    = LogUtils::CreateCliLogger("stride_logger", "stride_log.txt");
                logger->set_level(spdlog::level::off);

                auto household   = make_shared<Household>(2);
                auto contactPool = new ContactPool(0, ContactType::Id::Household);
                person           = make_shared<Person>();
                person->SetId(42);
                contactPool->AddMember(person.get());
                household->RegisterPool(contactPool);
                location = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
                location->AddCenter(household);
                auto pop = Population::Create();
                geoGrid  = make_shared<GeoGrid>(pop.get());
                geoGrid->AddLocation(location);

                community = make_shared<CommunityType>(1);
                auto pool = new ContactPool(1, ContactType::Id::Household);
                community->RegisterPool(pool);
        }

        void OneCommunityTest()
        {
                location->AddCenter(community);
                geoGrid->Finalize();

                populator->Apply(geoGrid, config);

                const auto& pools = community->GetPools();
                ASSERT_EQ(pools.size(), 1);
                EXPECT_EQ(pools[0]->GetPool().size(), 1);
                EXPECT_EQ((*pools[0]->begin())->GetId(), 42);
        }

        void ZeroCommunitiesTest()
        {
                auto pop = Population::Create();
                geoGrid  = make_shared<GeoGrid>(pop.get());
                geoGrid->Finalize();
                EXPECT_NO_THROW(populator->Apply(geoGrid, config));
        }

        void EmptyLocationTest()
        {
                auto pop = Population::Create();
                geoGrid  = make_shared<GeoGrid>(pop.get());
                location = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
                location->AddCenter(community);
                geoGrid->AddLocation(location);
                geoGrid->Finalize();
                EXPECT_NO_THROW(populator->Apply(geoGrid, config));
        }

        void TwoLocationsTest()
        {
                location->AddCenter(community);

                auto location2  = make_shared<Location>(2, 5, Coordinate(1, 1), "Brussel", 1500);
                auto community2 = make_shared<PrimaryCommunity>(1);
                auto pool       = new ContactPool(2, ContactType::Id::PrimaryCommunity);
                community2->RegisterPool(pool);
                location2->AddCenter(community2);

                geoGrid->AddLocation(location2);
                geoGrid->Finalize();
                populator->Apply(geoGrid, config);
                {
                        const auto& pools = community->GetPools();
                        ASSERT_EQ(pools.size(), 1);
                        EXPECT_EQ(pools[0]->GetPool().size(), 1);
                        EXPECT_EQ((*pools[0]->begin())->GetId(), 42);
                }
                {
                        const auto& pools = community2->GetPools();
                        ASSERT_EQ(pools.size(), 1);
                        EXPECT_EQ(pools[0]->GetPool().size(), 0);
                }
        }

        void OtherLocationTest()
        {
                auto location2 = make_shared<Location>(2, 5, Coordinate(1, 1), "Brussel", 1500);
                location2->AddCenter(community);
                geoGrid->AddLocation(location2);
                geoGrid->Finalize();

                populator->Apply(geoGrid, config);

                const auto& pools = community->GetPools();
                ASSERT_EQ(pools.size(), 1);
                EXPECT_EQ(pools[0]->GetPool().size(), 1);
                EXPECT_EQ((*pools[0]->begin())->GetId(), 42);
        }

        void HouseholdTest()
        {
                auto pool    = *location->RefCenters(Id::Household)[0]->begin();
                auto person2 = make_shared<Person>();
                person2->SetId(5);
                person2->SetAge(2);
                pool->AddMember(person2.get());
                location->AddCenter(community);

                auto community2 = make_shared<CommunityType>(2);
                community2->RegisterPool(new ContactPool(2, ContactType::Id::PrimaryCommunity));
                location->AddCenter(community2);

                geoGrid->Finalize();
                populator->Apply(geoGrid, config);

                HouseholdTestCheck(community2);
        }

        virtual void HouseholdTestCheck(shared_ptr<CommunityType> community2) = 0;

        shared_ptr<Populator>      populator;
        shared_ptr<RnMan>          rnManager;
        GeoGridConfig              config;
        shared_ptr<Location>       location;
        shared_ptr<CommunityType>  community;
        shared_ptr<GeoGrid>        geoGrid;
        shared_ptr<Person>         person;
        shared_ptr<spdlog::logger> logger;
};

class PrimaryCommunityPopulatorTest : public CommunityPopulatorTest<PrimaryCommunity>
{
public:
        PrimaryCommunityPopulatorTest() = default;

protected:
        void SetUp() override
        {
                CommunityPopulatorTest::SetUp();
                populator = make_shared<PrimaryCommunityPopulator>(*rnManager.get(), logger);
        }

        void HouseholdTestCheck(shared_ptr<PrimaryCommunity> community2) override
        {
                {
                        const auto& pools = community->GetPools();
                        ASSERT_EQ(pools.size(), 1);
                        EXPECT_EQ(pools[0]->GetPool().size(), 1);
                        EXPECT_EQ((*pools[0]->begin())->GetId(), 42);
                }
                {
                        const auto& pools = community2->GetPools();
                        ASSERT_EQ(pools.size(), 1);
                        EXPECT_EQ(pools[0]->GetPool().size(), 1);
                        EXPECT_EQ((*pools[0]->begin())->GetId(), 5);
                }
        }
};

class SecondaryCommunityPopulatorTest : public CommunityPopulatorTest<SecondaryCommunity>
{
public:
        SecondaryCommunityPopulatorTest() = default;

protected:
        void SetUp() override
        {
                CommunityPopulatorTest::SetUp();
                populator = make_shared<SecondaryCommunityPopulator>(*rnManager.get(), logger);
        }

        void HouseholdTestCheck(shared_ptr<SecondaryCommunity> community2) override
        {
                {
                        const auto& pools = community->GetPools();
                        ASSERT_EQ(pools.size(), 1);
                        EXPECT_EQ(pools[0]->GetPool().size(), 2);
                        EXPECT_EQ((*pools[0]->begin())->GetId(), 42);
                        EXPECT_EQ((*(pools[0]->begin() + 1))->GetId(), 5);
                }
                {
                        const auto& pools = community2->GetPools();
                        ASSERT_EQ(pools.size(), 1);
                        EXPECT_EQ(pools[0]->GetPool().size(), 0);
                }
        }
};

TEST_F(PrimaryCommunityPopulatorTest, OneCommunityTest) { OneCommunityTest(); }

TEST_F(PrimaryCommunityPopulatorTest, ZeroCommunitiesTest) { ZeroCommunitiesTest(); }

TEST_F(PrimaryCommunityPopulatorTest, EmptyLocationTest) { EmptyLocationTest(); }

TEST_F(PrimaryCommunityPopulatorTest, HouseholdTest) { HouseholdTest(); }

TEST_F(PrimaryCommunityPopulatorTest, TwoLocationsTest) { TwoLocationsTest(); }

TEST_F(PrimaryCommunityPopulatorTest, OtherLocationTest) { OtherLocationTest(); }

TEST_F(SecondaryCommunityPopulatorTest, OneCommunityTest) { OneCommunityTest(); }

TEST_F(SecondaryCommunityPopulatorTest, ZeroCommunitiesTest) { ZeroCommunitiesTest(); }

TEST_F(SecondaryCommunityPopulatorTest, EmptyLocationTest) { EmptyLocationTest(); }

TEST_F(SecondaryCommunityPopulatorTest, HouseholdTest) { HouseholdTest(); }

TEST_F(SecondaryCommunityPopulatorTest, TwoLocationsTest) { TwoLocationsTest(); }

TEST_F(SecondaryCommunityPopulatorTest, OtherLocationTest) { OtherLocationTest(); }
