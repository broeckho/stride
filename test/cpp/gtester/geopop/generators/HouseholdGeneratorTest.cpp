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

#include "geopop/generators/Generator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "geopop/PoolParams.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>
#include <array>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

class HouseholdGeneratorTest : public testing::Test
{
public:
        HouseholdGeneratorTest()
            : m_rn_man(RnInfo()), m_household_generator(m_rn_man), m_geogrid_config(), m_pop(Population::Create()),
              m_geo_grid(m_pop.get())
        {
        }

protected:
        RnMan                  m_rn_man;
        HouseholdGenerator     m_household_generator;
        GeoGridConfig          m_geogrid_config;
        shared_ptr<Population> m_pop;
        GeoGrid                m_geo_grid;
};

// Check that generator can handle situation with a single Location.
TEST_F(HouseholdGeneratorTest, OneLocationTest)
{
        m_geogrid_config.info.count_households = 4;

        auto loc1 = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        m_geo_grid.AddLocation(loc1);

        m_household_generator.Apply(m_geo_grid, m_geogrid_config);

        const auto& poolsOfLoc1 = loc1->CRefPools<Id::Household>();
        EXPECT_EQ(poolsOfLoc1.size(), 4);
}

// Check that generator can handle "no Locations" situation.
TEST_F(HouseholdGeneratorTest, ZeroLocationTest)
{
        m_geogrid_config.info.count_households = 4;
        m_household_generator.Apply(m_geo_grid, m_geogrid_config);

        EXPECT_EQ(m_geo_grid.size(), 0);
}

// check that generator can handle five Locations.
TEST_F(HouseholdGeneratorTest, FiveLocationsTest)
{
        m_geogrid_config.info.count_households = 4000;
        m_geogrid_config.param.pop_size           = 37542 * 100;

        auto loc1 = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 10150 * 100);
        auto loc2 = make_shared<Location>(2, 4, Coordinate(0, 0), "Vlaams-Brabant", 10040 * 100);
        auto loc3 = make_shared<Location>(3, 4, Coordinate(0, 0), "Henegouwen", 7460 * 100);
        auto loc4 = make_shared<Location>(4, 4, Coordinate(0, 0), "Limburg", 3269 * 100);
        auto loc5 = make_shared<Location>(5, 4, Coordinate(0, 0), "Luxemburg", 4123 * 100);

        m_geo_grid.AddLocation(loc1);
        m_geo_grid.AddLocation(loc2);
        m_geo_grid.AddLocation(loc3);
        m_geo_grid.AddLocation(loc4);
        m_geo_grid.AddLocation(loc5);

        for (const auto& loc : m_geo_grid) {
                loc->SetPopFraction(static_cast<double>(loc->GetPopCount()) /
                                    static_cast<double>(m_geogrid_config.param.pop_size));
        }

        m_household_generator.Apply(m_geo_grid, m_geogrid_config);

        array<unsigned int, 5> sizes{1179, 1137, 868, 358, 458};
        for (auto i = 0U; i < sizes.size(); i++) {
                EXPECT_EQ(sizes[i] * PoolParams<Id::Household>::pools, m_geo_grid[i]->CRefPools(Id::Household).size());
        }
}

} // namespace
