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
#include "geopop/HouseholdCenter.h"
#include "geopop/Location.h"
#include "geopop/PrimaryCommunityCenter.h"
#include "geopop/SecondaryCommunityCenter.h"
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
            : m_populator(), m_rn_man(), m_geogrid_config(), m_location(), m_community(), m_geo_grid(), m_person(),
              m_logger()
        {
        }

protected:
        void SetUp() override
        {
                m_rn_man = make_shared<RnMan>(RnInfo{});
                m_logger = LogUtils::CreateCliLogger("stride_logger", "stride_log.txt");
                m_logger->set_level(spdlog::level::off);

                auto household   = make_shared<HouseholdCenter>(2);
                auto contactPool = new ContactPool(0, ContactType::Id::Household);
                m_person         = make_shared<Person>();
                m_person->SetId(42);
                contactPool->AddMember(m_person.get());
                household->RegisterPool(contactPool);
                m_location = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
                m_location->AddCenter(household);
                auto pop   = Population::Create();
                m_geo_grid = make_shared<GeoGrid>(pop.get());
                m_geo_grid->AddLocation(m_location);

                m_community = make_shared<CommunityType>(1);
                auto pool   = new ContactPool(1, ContactType::Id::Household);
                m_community->RegisterPool(pool);
        }

        void OneCommunityTest()
        {
                m_location->AddCenter(m_community);
                m_geo_grid->Finalize();

                m_populator->Apply(*m_geo_grid, m_geogrid_config);

                ASSERT_EQ(m_community->size(), 1);
                EXPECT_EQ((*m_community)[0]->size(), 1);
                EXPECT_EQ((*(*m_community)[0])[0]->GetId(), 42);
        }

        void ZeroCommunitiesTest()
        {
                auto pop   = Population::Create();
                m_geo_grid = make_shared<GeoGrid>(pop.get());
                m_geo_grid->Finalize();
                EXPECT_NO_THROW(m_populator->Apply(*m_geo_grid, m_geogrid_config));
        }

        void EmptyLocationTest()
        {
                auto pop   = Population::Create();
                m_geo_grid = make_shared<GeoGrid>(pop.get());
                m_location = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
                m_location->AddCenter(m_community);
                m_geo_grid->AddLocation(m_location);
                m_geo_grid->Finalize();
                EXPECT_NO_THROW(m_populator->Apply(*m_geo_grid, m_geogrid_config));
        }

        void TwoLocationsTest()
        {
                m_location->AddCenter(m_community);

                auto location2  = make_shared<Location>(2, 5, Coordinate(1, 1), "Brussel", 1500);
                auto community2 = make_shared<PrimaryCommunityCenter>(1);
                auto pool       = new ContactPool(2, ContactType::Id::PrimaryCommunity);
                community2->RegisterPool(pool);
                location2->AddCenter(community2);

                m_geo_grid->AddLocation(location2);
                m_geo_grid->Finalize();
                m_populator->Apply(*m_geo_grid, m_geogrid_config);
                {
                        ASSERT_EQ(m_community->size(), 1);
                        EXPECT_EQ((*m_community)[0]->size(), 1);
                        EXPECT_EQ(((*(*m_community)[0])[0])->GetId(), 42);
                }
                {
                        ASSERT_EQ(community2->size(), 1);
                        EXPECT_EQ((*community2)[0]->size(), 0);
                }
        }

        void OtherLocationTest()
        {
                auto location2 = make_shared<Location>(2, 5, Coordinate(1, 1), "Brussel", 1500);
                location2->AddCenter(m_community);
                m_geo_grid->AddLocation(location2);
                m_geo_grid->Finalize();

                m_populator->Apply(*m_geo_grid, m_geogrid_config);

                ASSERT_EQ(m_community->size(), 1);
                EXPECT_EQ((*m_community)[0]->size(), 1);
                EXPECT_EQ((*(*m_community)[0])[0]->GetId(), 42);
        }

        void HouseholdTest()
        {
                auto pool    = *m_location->RefCenters(Id::Household)[0]->begin();
                auto person2 = make_shared<Person>();
                person2->SetId(5);
                person2->SetAge(2);
                pool->AddMember(person2.get());
                m_location->AddCenter(m_community);

                auto community2 = make_shared<CommunityType>(2);
                community2->RegisterPool(new ContactPool(2, ContactType::Id::PrimaryCommunity));
                m_location->AddCenter(community2);

                m_geo_grid->Finalize();
                m_populator->Apply(*m_geo_grid, m_geogrid_config);

                HouseholdTestCheck(community2);
        }

        virtual void HouseholdTestCheck(shared_ptr<CommunityType> community2) = 0;

        shared_ptr<Populator>      m_populator;
        shared_ptr<RnMan>          m_rn_man;
        GeoGridConfig              m_geogrid_config;
        shared_ptr<Location>       m_location;
        shared_ptr<CommunityType>  m_community;
        shared_ptr<GeoGrid>        m_geo_grid;
        shared_ptr<Person>         m_person;
        shared_ptr<spdlog::logger> m_logger;
};

class PrimaryCommunityPopulatorTest : public CommunityPopulatorTest<PrimaryCommunityCenter>
{
public:
        PrimaryCommunityPopulatorTest() = default;

protected:
        void SetUp() override
        {
                CommunityPopulatorTest::SetUp();
                m_populator = make_shared<PrimaryCommunityPopulator>(*m_rn_man.get(), m_logger);
        }

        void HouseholdTestCheck(shared_ptr<PrimaryCommunityCenter> community2) override
        {
                {
                        ASSERT_EQ(m_community->size(), 1);
                        EXPECT_EQ((*m_community)[0]->size(), 1);
                        EXPECT_EQ((*(*m_community)[0])[0]->GetId(), 42);
                }
                {
                        ASSERT_EQ(community2->size(), 1);
                        EXPECT_EQ((*community2)[0]->size(), 1);
                        EXPECT_EQ((*(*community2)[0])[0]->GetId(), 5);
                }
        }
};

class SecondaryCommunityPopulatorTest : public CommunityPopulatorTest<SecondaryCommunityCenter>
{
public:
        SecondaryCommunityPopulatorTest() = default;

protected:
        void SetUp() override
        {
                CommunityPopulatorTest::SetUp();
                m_populator = make_shared<SecondaryCommunityPopulator>(*m_rn_man.get(), m_logger);
        }

        void HouseholdTestCheck(shared_ptr<SecondaryCommunityCenter> community2) override
        {
                {
                        ASSERT_EQ(m_community->size(), 1);
                        EXPECT_EQ((*m_community)[0]->size(), 2);
                        EXPECT_EQ((*(*m_community)[0])[0]->GetId(), 42);
                        EXPECT_EQ((*(*m_community)[0])[1]->GetId(), 5);
                }
                {
                        ASSERT_EQ(community2->size(), 1);
                        EXPECT_EQ((*community2)[0]->size(), 0);
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
