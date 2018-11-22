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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#include <gengeopop/GeoGridConfig.h>
#include <gengeopop/generators/WorkplaceGenerator.h>
#include <gtest/gtest.h>
#include <tuple>
#include <util/RnMan.h>

#include "../../createlogger.h"
using namespace gengeopop;

namespace {

TEST(WorkplaceGeneratorTest, ZeroLocationTest)
{

        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplaceGenerator workplaceGenerator(rnManager, CreateLogger());
        GeoGridConfig      config{};
        config.input.populationSize               = 10000;
        config.calculated._1826_years_and_student = 20000;

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());
        workplaceGenerator.Apply(geoGrid, config);

        EXPECT_EQ(geoGrid->size(), 0);
}

TEST(WorkplaceGeneratorTest, NoCommuting)
{

        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplaceGenerator workplaceGenerator(rnManager, CreateLogger());
        GeoGridConfig      config{};
        config.input.populationSize                  = 5 * 1000 * 1000;
        config.calculated._1865_and_years_active     = static_cast<unsigned int>(0.20 * 5 * 1000 * 1000);
        config.input.fraction_1865_years_active      = 0.20;
        config.input.fraction_active_commutingPeople = 0;

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());

        std::vector<int> sizes{128331, 50784,  191020, 174476, 186595, 105032, 136388, 577,   111380, 171014,
                               63673,  49438,  45590,  164666, 185249, 141389, 82525,  40397, 123307, 168128,
                               172937, 15581,  22891,  143505, 178516, 116959, 144659, 20775, 156009, 91951,
                               49823,  181594, 119075, 27700,  116959, 146583, 102531, 58864, 76946,  91951,
                               134464, 59248,  10003,  125423, 15004,  8656,   13658,  50784, 61749,  165243};
        for (int size : sizes) {
                geoGrid->AddLocation(
                    std::make_shared<Location>(1, 4, size, Coordinate(0, 0), "Size: " + std::to_string(size)));
        }

        workplaceGenerator.Apply(geoGrid, config);

        std::vector<int> expectedWorkplaceCount{
            1342, 512,  1948, 1801, 1919, 1087, 1304, 6,    1133, 1728, 646, 441,  450, 1643, 1897, 1410, 810,
            382,  1192, 1688, 1691, 161,  204,  1433, 1796, 1187, 1449, 201, 1540, 923, 452,  1756, 1167, 261,
            1197, 1455, 1058, 594,  796,  868,  1355, 594,  104,  1298, 136, 94,   140, 500,  588,  1663};

        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expectedWorkplaceCount[i], geoGrid->Get(i)->GetContactCenters().size());
        }
}

TEST(WorkplaceGeneratorTest, AbsNullCommuting)
{

        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplaceGenerator workplaceGenerator(rnManager, CreateLogger());
        GeoGridConfig      config{};
        config.input.populationSize                  = 5 * 1000 * 1000;
        config.calculated._1865_and_years_active     = static_cast<unsigned int>(0.20 * 5 * 1000 * 1000);
        config.input.fraction_1865_years_active      = 0.20;
        config.input.fraction_active_commutingPeople = 0.10;

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());

        std::vector<int> sizes{128331, 50784,  191020, 174476, 186595, 105032, 136388, 577,   111380, 171014,
                               63673,  49438,  45590,  164666, 185249, 141389, 82525,  40397, 123307, 168128,
                               172937, 15581,  22891,  143505, 178516, 116959, 144659, 20775, 156009, 91951,
                               49823,  181594, 119075, 27700,  116959, 146583, 102531, 58864, 76946,  91951,
                               134464, 59248,  10003,  125423, 15004,  8656,   13658,  50784, 61749,  165243};
        for (int size : sizes) {
                geoGrid->AddLocation(
                    std::make_shared<Location>(1, 4, size, Coordinate(0, 0), "Size: " + std::to_string(size)));
        }

        // 10% of the pop of loc0 is commuting to loc1 = 12.833 people
        geoGrid->Get(0)->AddOutgoingCommutingLocation(geoGrid->Get(1), 0.10); // 0.10 is relative to loc0
        geoGrid->Get(1)->AddIncomingCommutingLocation(geoGrid->Get(0), 0.10); // 0.10 is relative to loc0

        // 25,26% of the pop of loc1 is commuting to loc0 = 12.833 people
        geoGrid->Get(1)->AddOutgoingCommutingLocation(geoGrid->Get(0), 0.252697700063012); // 0.25 is relative to loc1
        geoGrid->Get(0)->AddIncomingCommutingLocation(geoGrid->Get(1), 0.252697700063012); // 0.25 is relative to loc1

        EXPECT_EQ(1283, geoGrid->Get(0)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(1283, geoGrid->Get(0)->OutGoingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(1283, geoGrid->Get(1)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(1283, geoGrid->Get(1)->OutGoingCommutingPeople(config.input.fraction_active_commutingPeople));

        // -> shouldn't change the test outcome in comparision with the previous test

        workplaceGenerator.Apply(geoGrid, config);

        std::vector<int> expectedWorkplaceCount{
            1351, 521,  1960, 1798, 1907, 1088, 1301, 5,    1134, 1739, 644, 431,  447, 1650, 1894, 1409, 809,
            377,  1198, 1685, 1692, 155,  210,  1430, 1793, 1191, 1449, 203, 1536, 928, 446,  1754, 1169, 263,
            1194, 1456, 1058, 594,  793,  869,  1356, 591,  105,  1297, 136, 95,   139, 499,  588,  1663};
        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expectedWorkplaceCount[i], geoGrid->Get(i)->GetContactCenters().size());
        }
}

TEST(WorkplaceGeneratorTest, TenCommuting)
{

        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplaceGenerator workplaceGenerator(rnManager, CreateLogger());
        GeoGridConfig      config{};
        config.input.populationSize                  = 5 * 1000 * 1000;
        config.calculated._1865_and_years_active     = static_cast<unsigned int>(0.20 * 5 * 1000 * 1000);
        config.input.fraction_1865_years_active      = 0.20;
        config.input.fraction_active_commutingPeople = 0.10;

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());

        std::vector<int> sizes{128331, 50784,  191020, 174476, 186595, 105032, 136388, 577,   111380, 171014,
                               63673,  49438,  45590,  164666, 185249, 141389, 82525,  40397, 123307, 168128,
                               172937, 15581,  22891,  143505, 178516, 116959, 144659, 20775, 156009, 91951,
                               49823,  181594, 119075, 27700,  116959, 146583, 102531, 58864, 76946,  91951,
                               134464, 59248,  10003,  125423, 15004,  8656,   13658,  50784, 61749,  165243};
        for (int size : sizes) {
                geoGrid->AddLocation(
                    std::make_shared<Location>(1, 4, size, Coordinate(0, 0), "Size: " + std::to_string(size)));
        }

        std::vector<std::tuple<size_t, size_t, double>> commuting{
            // loc0 to loc10 -> 0.23 of loc0
            {0, 10, 0.23}, {25, 3, 0.43}, {38, 17, 0.65}, {10, 17, 0.25}, {15, 17, 0.22}, {27, 17, 0.47},
        };

        for (const std::tuple<size_t, size_t, double>& info : commuting) {
                geoGrid->Get(std::get<0>(info))
                    ->AddOutgoingCommutingLocation(geoGrid->Get(std::get<1>(info)), std::get<2>(info));
                geoGrid->Get(std::get<1>(info))
                    ->AddIncomingCommutingLocation(geoGrid->Get(std::get<0>(info)), std::get<2>(info));
        }

        EXPECT_EQ(2951, geoGrid->Get(0)->OutGoingCommutingPeople(
                            config.input.fraction_active_commutingPeople)); // = 0,23 * 128331 * 0,10 = 2951,613
        EXPECT_EQ(0, geoGrid->Get(0)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(1591, geoGrid->Get(10)->OutGoingCommutingPeople(
                            config.input.fraction_active_commutingPeople)); // = 0,25 * 63673 * 0,10 = 1591,825
        EXPECT_EQ(2951, geoGrid->Get(10)->IncomingCommutingPeople(
                            config.input.fraction_active_commutingPeople)); // = 0,23 * 128331 * 0,10 = 2951,613
        EXPECT_EQ(5029, geoGrid->Get(25)->OutGoingCommutingPeople(
                            config.input.fraction_active_commutingPeople)); // = 0,43 * 116959 * 0,10 = 5029,023
        EXPECT_EQ(0, geoGrid->Get(25)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(0, geoGrid->Get(3)->OutGoingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(5029, geoGrid->Get(3)->IncomingCommutingPeople(
                            config.input.fraction_active_commutingPeople)); // = 0,43 * 116959 * 0,10 = 5029,023
        EXPECT_EQ(0, geoGrid->Get(17)->OutGoingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(
            10680,
            geoGrid->Get(17)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople)); // = 0,10 * (0,65
                                                                                                      // * 76946 + 0,22
                                                                                                      // * 141389 + 0,47
                                                                                                      // * 20775 + 0,25*
                                                                                                      // 63673) =
                                                                                                      // 10680,298
        EXPECT_EQ(5001, geoGrid->Get(38)->OutGoingCommutingPeople(
                            config.input.fraction_active_commutingPeople)); // = 0,65 * 76946 * 0,10 = 5001,048
        EXPECT_EQ(0, geoGrid->Get(38)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(3110, geoGrid->Get(15)->OutGoingCommutingPeople(
                            config.input.fraction_active_commutingPeople)); // 0,22 * 141389 * 0,10 = 3310,558
        EXPECT_EQ(0, geoGrid->Get(15)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(0, geoGrid->Get(17)->OutGoingCommutingPeople(config.input.fraction_active_commutingPeople));
        EXPECT_EQ(
            10680,
            geoGrid->Get(17)->IncomingCommutingPeople(config.input.fraction_active_commutingPeople)); // = 0,10 * (0,65
                                                                                                      // * 76946 + 0,22
                                                                                                      // * 141389 + 0,47
                                                                                                      // * 20775 + 0,25*
                                                                                                      // 63673) =
                                                                                                      // 10680,298

        workplaceGenerator.Apply(geoGrid, config);

        std::vector<int> expectedWorkplaceCount{
            1328, 516,  1941, 1850, 1906, 1087, 1297, 6,    1132, 1727, 671, 428,  447, 1647, 1896, 1394, 810,
            464,  1220, 1682, 1672, 149,  211,  1423, 1802, 1185, 1429, 213, 1530, 917, 446,  1760, 1155, 274,
            1190, 1458, 1046, 593,  772,  873,  1355, 589,  101,  1291, 142, 93,   132, 507,  584,  1659};
        ;
        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expectedWorkplaceCount[i], geoGrid->Get(i)->GetContactCenters().size());
        }

        std::cout << std::endl;
}

} // namespace
