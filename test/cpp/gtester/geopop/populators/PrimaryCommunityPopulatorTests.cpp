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
 *  Copyright 2019, Jan Broeckhove.
 */

#include "geopop/generators/HouseholdGenerator.h"
#include "geopop/generators/PrimaryCommunityGenerator.h"
#include "geopop/populators/PrimaryCommunityPopulator.h"

#include "geopop/Coordinate.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

class PrimaryCommunityPopulatorTest : public testing::Test
{
public:
        PrimaryCommunityPopulatorTest()
            : m_rn_man(RnInfo{}), m_populator(m_rn_man), m_geogrid_config(), m_pop(Population::Create()),
              m_location(make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500)),
              m_geo_grid(m_pop->RefGeoGrid()), m_person(), m_community_generator(m_rn_man),
              m_household_generator(m_rn_man)
        {
        }

protected:
        // Set initial situation: a one-person (with Id 42) household at Location 'm_location' registered
        // in the population/geogrid and one PrimaryCommunity 'm_community_cc' constructed but having no
        // members and not yet registered in the GeoGrid.
        void SetUp() override
        {
                m_household_generator.AddPools(*m_location, m_pop.get(), m_pppc);

                m_person = make_shared<Person>();
                m_person->SetId(42);
                m_location->RefPools(Id::Household)[0]->AddMember(m_person.get());

                m_geo_grid.AddLocation(m_location);

                m_community_generator.AddPools(*m_location, m_pop.get(), m_pppc);
        }

        RnMan                     m_rn_man;
        PrimaryCommunityPopulator m_populator;
        GeoGridConfig             m_geogrid_config;
        shared_ptr<Population>    m_pop;
        shared_ptr<Location>      m_location;
        GeoGrid&                  m_geo_grid;
        shared_ptr<Person>        m_person;
        PrimaryCommunityGenerator m_community_generator;
        HouseholdGenerator        m_household_generator;
        const unsigned int        m_pphh = GeoGridConfig{}.pools.pools_per_household;
        const unsigned int        m_pppc = GeoGridConfig{}.pools.pools_per_primary_community;
};

TEST_F(PrimaryCommunityPopulatorTest, OneCommunityTest)
{
        m_geo_grid.Finalize();

        m_populator.Apply(m_geo_grid, m_geogrid_config);

        auto& scPools = m_location->RefPools(Id::PrimaryCommunity);
        ASSERT_EQ(scPools.size(), 1);
        EXPECT_EQ(scPools[0]->size(), 1);
        EXPECT_EQ((*scPools[0])[0]->GetId(), 42);
}

TEST_F(PrimaryCommunityPopulatorTest, EmptyCommunityTest)
{
        m_geo_grid.Finalize();

        EXPECT_NO_THROW(m_populator.Apply(m_geo_grid, m_geogrid_config));
}

// At this Location: a two-person household, each person assigned to different PrimaryCommunity.
TEST_F(PrimaryCommunityPopulatorTest, HouseholdTest)
{
        auto person2 = make_shared<Person>();
        person2->SetId(5);
        person2->SetAge(2);

        auto pool = m_location->RefPools(Id::Household)[0];
        pool->AddMember(person2.get());

        m_community_generator.AddPools(*m_location, m_pop.get(), m_pppc);

        m_geo_grid.Finalize();
        m_populator.Apply(m_geo_grid, m_geogrid_config);

        auto& scPools = m_location->RefPools(Id::PrimaryCommunity);
        ASSERT_EQ(scPools.size(), 2);
        EXPECT_EQ(scPools[0]->size(), 1);
        EXPECT_EQ((*scPools[0])[0]->GetId(), 42);
        EXPECT_EQ(scPools[1]->size(), 1);
        EXPECT_EQ((*scPools[1])[0]->GetId(), 5);
}

// Two Locations each with a PrimaryCommunity, a one-person household at the first location
// and that person gets assigned to the PrimaryCommunity at the first Location.
TEST_F(PrimaryCommunityPopulatorTest, TwoLocationsTest)
{
        auto location2 = make_shared<Location>(2, 5, Coordinate(1, 1), "Brussel", 1500);
        m_community_generator.AddPools(*location2, m_pop.get(), m_pppc);

        m_geo_grid.AddLocation(location2);
        m_geo_grid.Finalize();
        m_populator.Apply(m_geo_grid, m_geogrid_config);

        auto& scPools = m_location->RefPools(Id::PrimaryCommunity);
        ASSERT_EQ(scPools.size(), 1);
        EXPECT_EQ(scPools[0]->size(), 1);
        EXPECT_EQ((*scPools[0])[0]->GetId(), 42);
}
