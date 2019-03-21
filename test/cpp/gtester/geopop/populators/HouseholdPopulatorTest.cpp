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

#include "geopop/populators/HouseholdPopulator.h"
#include "geopop/generators/HouseholdGenerator.h"

#include "geopop/Coordinate.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
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
        HouseholdPopulatorTest()
                : m_rn_man(RnInfo()), m_household_populator(m_rn_man), m_geogrid_config(), m_pop(Population::Create()),
                  m_geo_grid(m_pop->RefGeoGrid()), m_household_generator(m_rn_man)
        {}

protected:
        RnMan                        m_rn_man;
        HouseholdPopulator            m_household_populator;
        GeoGridConfig                m_geogrid_config;
        shared_ptr<Population>       m_pop;
        GeoGrid&                     m_geo_grid;
        HouseholdGenerator           m_household_generator;
};

TEST_F(HouseholdPopulatorTest, OneHouseholdTest)
{
        m_geogrid_config.refHH.ages = vector<vector<unsigned int>>{{8U}};

        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        auto hCenter = make_shared<ContactCenter>(1, Id::Household);
        m_household_generator.SetupPools(*loc1, *hCenter, m_geogrid_config, m_pop.get());
        loc1->AddCenter(hCenter);
        m_geo_grid.AddLocation(loc1);

        m_household_populator.Apply(m_geo_grid, m_geogrid_config);

        ASSERT_EQ(hCenter->size(), 1);
        EXPECT_EQ((*hCenter)[0]->size(), 1);

        const auto& hPools = loc1->RefPools(Id::Household);
        const auto& pool1 = *hPools[0];
        ASSERT_EQ(hPools.size(), 1);
        ASSERT_EQ(pool1.size(), 1);
}

TEST_F(HouseholdPopulatorTest, ZeroHouseholdsTest)
{
        EXPECT_NO_THROW(m_household_populator.Apply(m_geo_grid, m_geogrid_config));
}

TEST_F(HouseholdPopulatorTest, FiveHouseholdsTest)
{
        m_geogrid_config.refHH.ages = vector<vector<unsigned int>>{{18U}};

        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);

        for (unsigned int i = 0U; i < 5U; ++i) {
                auto household = make_shared<ContactCenter>(i, Id::Household);
                m_household_generator.SetupPools(*loc1, *household, m_geogrid_config, m_pop.get());
                loc1->AddCenter(household);
        }

        m_geo_grid.AddLocation(loc1);
        m_household_populator.Apply(m_geo_grid, m_geogrid_config);

        for (const auto& hCenter : loc1->RefCenters(Id::Household)) {
                ASSERT_EQ(hCenter->size(), 1);
                ASSERT_EQ((*hCenter)[0]->size(), 1);
                EXPECT_EQ((*(*hCenter)[0]->begin())->GetAge(), 18);
        }

        for (const auto& hPool : loc1->RefPools(Id::Household)) {
                ASSERT_EQ(hPool->size(), 1);
                EXPECT_EQ((*hPool)[0]->GetAge(), 18);
        }
}

TEST_F(HouseholdPopulatorTest, MultipleHouseholdTypesTest)
{
        m_geogrid_config.refHH.ages = vector<vector<unsigned int>>{{18U}, {12U, 56U}};

        const auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);

        const auto hCenter = make_shared<ContactCenter>(1, Id::Household);
        m_household_generator.SetupPools(*loc1, *hCenter, m_geogrid_config, m_pop.get());
        loc1->AddCenter(hCenter);


        const auto hCenter2 = make_shared<ContactCenter>(2, Id::Household);
        m_household_generator.SetupPools(*loc1, *hCenter2, m_geogrid_config, m_pop.get());
        loc1->AddCenter(hCenter2);

        m_geo_grid.AddLocation(loc1);
        m_household_populator.Apply(m_geo_grid, m_geogrid_config);

        ASSERT_EQ(hCenter->size(), 1);
        EXPECT_EQ((*hCenter)[0]->size(), 1);
        EXPECT_EQ((*(*hCenter)[0]->begin())->GetAge(), 18);

        ASSERT_EQ(hCenter2->size(), 1);
        EXPECT_EQ((*hCenter2)[0]->size(), 2);
        EXPECT_EQ((*(*hCenter2)[0]->begin())->GetAge(), 12);
        EXPECT_EQ((*(*hCenter2)[0]->begin() + 1)->GetAge(), 56);

        const auto& hPools = loc1->RefPools(Id::Household);
        const auto& pool1 = *hPools[0];
        const auto& pool2 = *hPools[1];

        ASSERT_EQ(hPools.size(), 2);
        ASSERT_EQ(pool1.size(), 1);
        EXPECT_EQ(pool1[0]->GetAge(), 18);
        ASSERT_EQ(pool2.size(), 2);
        EXPECT_EQ(pool2[0]->GetAge(), 12);
        EXPECT_EQ(pool2[1]->GetAge(), 56);
}
