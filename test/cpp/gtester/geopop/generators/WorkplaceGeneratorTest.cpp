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

#include "geopop/generators/WorkplaceGenerator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>
#include <tuple>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

class WorkplaceGeneratorTest : public testing::Test {
public:
        WorkplaceGeneratorTest()
                : m_rn_man(RnInfo()), m_workplace_generator(m_rn_man), m_geogrid_config(),
                  m_pop(Population::Create()), m_geo_grid(m_pop.get())
        {
        }

protected:
        RnMan                        m_rn_man;
        WorkplaceGenerator           m_workplace_generator;
        GeoGridConfig                m_geogrid_config;
        shared_ptr<Population>       m_pop;
        GeoGrid                      m_geo_grid;
};

// Check that generator can handle empty GeoGrid.
TEST_F(WorkplaceGeneratorTest, ZeroLocationTest)
{
        m_geogrid_config.input.pop_size           = 10000;
        m_geogrid_config.popInfo.popcount_college = 20000;

        unsigned int       ccCounter{1U};
        m_workplace_generator.Apply(m_geo_grid, m_geogrid_config, ccCounter);

        EXPECT_EQ(m_geo_grid.size(), 0);
}

// Check that situation without commutes is OK.
TEST_F(WorkplaceGeneratorTest, NoCommuting)
{
        m_geogrid_config.input.pop_size                     = 5 * 1000 * 1000;
        m_geogrid_config.popInfo.popcount_workplace         = static_cast<unsigned int>(0.20 * 5 * 1000 * 1000);
        m_geogrid_config.input.particpation_workplace       = 0.20;
        m_geogrid_config.input.fraction_workplace_commuters = 0;

        vector<int> sizes{128331, 50784,  191020, 174476, 186595, 105032, 136388, 577,   111380, 171014,
                          63673,  49438,  45590,  164666, 185249, 141389, 82525,  40397, 123307, 168128,
                          172937, 15581,  22891,  143505, 178516, 116959, 144659, 20775, 156009, 91951,
                          49823,  181594, 119075, 27700,  116959, 146583, 102531, 58864, 76946,  91951,
                          134464, 59248,  10003,  125423, 15004,  8656,   13658,  50784, 61749,  165243};
        for (int size : sizes) {
                m_geo_grid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Size: " + to_string(size), size));
        }

        unsigned int       ccCounter{1U};
        m_workplace_generator.Apply(m_geo_grid, m_geogrid_config, ccCounter);

        vector<int> expected{1342, 512,  1948, 1801, 1919, 1087, 1304, 6,    1133, 1728, 646,  441,  450,
                             1643, 1897, 1410, 810,  382,  1192, 1688, 1691, 161,  204,  1433, 1796, 1187,
                             1449, 201,  1540, 923,  452,  1756, 1167, 261,  1197, 1455, 1058, 594,  796,
                             868,  1355, 594,  104,  1298, 136,  94,   140,  500,  588,  1663};

        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expected[i], m_geo_grid[i]->CRefCenters(Id::Workplace).size());
                EXPECT_EQ(expected[i] * m_geogrid_config.pools.pools_per_workplace,
                                                        m_geo_grid[i]->CRefPools(Id::Workplace).size() );
        }
}

// As many commutes from A to B as from B to A.
TEST_F(WorkplaceGeneratorTest, NullCommuting)
{
        m_geogrid_config.input.pop_size                     = 5 * 1000 * 1000;
        m_geogrid_config.popInfo.popcount_workplace         = static_cast<unsigned int>(0.20 * 5 * 1000 * 1000);
        m_geogrid_config.input.particpation_workplace       = 0.20;
        m_geogrid_config.input.fraction_workplace_commuters = 0.10;

        vector<int> sizes{128331, 50784,  191020, 174476, 186595, 105032, 136388, 577,   111380, 171014,
                          63673,  49438,  45590,  164666, 185249, 141389, 82525,  40397, 123307, 168128,
                          172937, 15581,  22891,  143505, 178516, 116959, 144659, 20775, 156009, 91951,
                          49823,  181594, 119075, 27700,  116959, 146583, 102531, 58864, 76946,  91951,
                          134464, 59248,  10003,  125423, 15004,  8656,   13658,  50784, 61749,  165243};
        for (int size : sizes) {
                m_geo_grid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Size: " + to_string(size), size));
        }

        // 10% of the pop of loc0 is commuting to loc1 = 12.833 people
        m_geo_grid[0]->AddOutgoingCommute(m_geo_grid[1], 0.10); // 0.10 is relative to loc0
        m_geo_grid[1]->AddIncomingCommute(m_geo_grid[0], 0.10); // 0.10 is relative to loc0

        // 25,26% of the pop of loc1 is commuting to loc0 = 12.833 people
        m_geo_grid[1]->AddOutgoingCommute(m_geo_grid[0], 0.252697700063012); // 0.25 is relative to loc1
        m_geo_grid[0]->AddIncomingCommute(m_geo_grid[1], 0.252697700063012); // 0.25 is relative to loc1

        EXPECT_EQ(1283, m_geo_grid[0]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(1283, m_geo_grid[0]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(1283, m_geo_grid[1]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(1283, m_geo_grid[1]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        unsigned int       ccCounter{1U};
        m_workplace_generator.Apply(m_geo_grid, m_geogrid_config, ccCounter);

        vector<int> expected{1351, 521,  1960, 1798, 1907, 1088, 1301, 5,    1134, 1739, 644,  431,  447,
                             1650, 1894, 1409, 809,  377,  1198, 1685, 1692, 155,  210,  1430, 1793, 1191,
                             1449, 203,  1536, 928,  446,  1754, 1169, 263,  1194, 1456, 1058, 594,  793,
                             869,  1356, 591,  105,  1297, 136,  95,   139,  499,  588,  1663};
        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expected[i], m_geo_grid[i]->CRefCenters(Id::Workplace).size());
                EXPECT_EQ(expected[i] * m_geogrid_config.pools.pools_per_workplace,
                          m_geo_grid[i]->CRefPools(Id::Workplace).size() );
        }
}

TEST_F(WorkplaceGeneratorTest, TenCommuting)
{
        m_geogrid_config.input.pop_size                     = 5 * 1000 * 1000;
        m_geogrid_config.popInfo.popcount_workplace         = static_cast<unsigned int>(0.20 * 5 * 1000 * 1000);
        m_geogrid_config.input.particpation_workplace       = 0.20;
        m_geogrid_config.input.fraction_workplace_commuters = 0.10;

        vector<int> sizes{128331, 50784,  191020, 174476, 186595, 105032, 136388, 577,   111380, 171014,
                          63673,  49438,  45590,  164666, 185249, 141389, 82525,  40397, 123307, 168128,
                          172937, 15581,  22891,  143505, 178516, 116959, 144659, 20775, 156009, 91951,
                          49823,  181594, 119075, 27700,  116959, 146583, 102531, 58864, 76946,  91951,
                          134464, 59248,  10003,  125423, 15004,  8656,   13658,  50784, 61749,  165243};
        for (int size : sizes) {
                m_geo_grid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Size: " + to_string(size), size));
        }

        vector<tuple<size_t, size_t, double>> commuting;
        commuting.emplace_back(make_tuple(0, 10, 0.23));
        commuting.emplace_back(make_tuple(25, 3, 0.43));
        commuting.emplace_back(make_tuple(38, 17, 0.65));
        commuting.emplace_back(make_tuple(10, 17, 0.25));
        commuting.emplace_back(make_tuple(15, 17, 0.22));
        commuting.emplace_back(make_tuple(27, 17, 0.47));

        for (const tuple<size_t, size_t, double>& info : commuting) {
                m_geo_grid[get<0>(info)]->AddOutgoingCommute(m_geo_grid[get<1>(info)], get<2>(info));
                m_geo_grid[get<1>(info)]->AddIncomingCommute(m_geo_grid[get<0>(info)], get<2>(info));
        }

        // = 0,23 * 128331 * 0,10 = 2951,613
        EXPECT_EQ(2951, m_geo_grid[0]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(0, m_geo_grid[0]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // = 0,25 * 63673 * 0,10 = 1591,825
        EXPECT_EQ(1591, m_geo_grid[10]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // = 0,23 * 128331 * 0,10 = 2951,613
        EXPECT_EQ(2951, m_geo_grid[10]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // = 0,43 * 116959 * 0,10 = 5029,023
        EXPECT_EQ(5029, m_geo_grid[25]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(0, m_geo_grid[25]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(0, m_geo_grid[3]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // = 0,43 * 116959 * 0,10 = 5029,023
        EXPECT_EQ(5029, m_geo_grid[3]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(0, m_geo_grid[17]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // = 0,10 * (0,65 * 76946 + 0,22 * 141389 + 0,47 * 20775 + 0,25*63673) = 10680,298
        EXPECT_EQ(10680, m_geo_grid[17]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // = 0,65 * 76946 * 0,10 = 5001,048
        EXPECT_EQ(5001, m_geo_grid[38]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(0, m_geo_grid[38]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // 0,22 * 141389 * 0,10 = 3310,558
        EXPECT_EQ(3110, m_geo_grid[15]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(0, m_geo_grid[15]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));
        EXPECT_EQ(0, m_geo_grid[17]->GetOutgoingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        // = 0,10 * (0,65 * 76946 + 0,22  * 141389 + 0,47 * 20775 + 0,25* 63673) = 10680,298
        EXPECT_EQ(10680, m_geo_grid[17]->GetIncomingCommuteCount(m_geogrid_config.input.fraction_workplace_commuters));

        unsigned int       ccCounter{1U};
        m_workplace_generator.Apply(m_geo_grid, m_geogrid_config, ccCounter);

        vector<int> expected{1328, 516,  1941, 1850, 1906, 1087, 1297, 6,    1132, 1727, 671,  428,  447,
                             1647, 1896, 1394, 810,  464,  1220, 1682, 1672, 149,  211,  1423, 1802, 1185,
                             1429, 213,  1530, 917,  446,  1760, 1155, 274,  1190, 1458, 1046, 593,  772,
                             873,  1355, 589,  101,  1291, 142,  93,   132,  507,  584,  1659};

        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expected[i], m_geo_grid[i]->CRefCenters(Id::Workplace).size());
                EXPECT_EQ(expected[i] * m_geogrid_config.pools.pools_per_workplace,
                          m_geo_grid[i]->CRefPools(Id::Workplace).size() );
        }
}

} // namespace
