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
 *  Copyright 2018 Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Implementation of scenario tests running in batch mode.
 */

#include "util/RnPcg.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <chrono>
#include <exception>
#include <gtest/gtest.h>
#include <iostream>
#include <pcg/pcg_random.hpp>
#include <pcg/randutils.hpp>
#include <random>
#include <sstream>
#include <thread>

using namespace std;
using namespace ::testing;
using namespace randutils;
using namespace stride::util;

namespace Tests {
using pcg_extras::operator<<;

TEST(UnitRnMan, DefaultInfo)
{
        const RnPcg::Info info;

        EXPECT_EQ("1,2,3,4", info.m_seed_seq_init);
        EXPECT_EQ("", info.m_state);
        EXPECT_EQ(1u, info.m_stream_count);
}

TEST(UnitRnMan, GetInfo1)
{
        RnPcg      rnPcg;
        const auto info = rnPcg.GetInfo();

        pcg64        engine1(seed_seq_fe128{1, 2, 3, 4});
        stringstream ss;
        ss << engine1 << " ";

        EXPECT_EQ("1,2,3,4", info.m_seed_seq_init);
        EXPECT_EQ(ss.str(), info.m_state);
        EXPECT_EQ(1u, info.m_stream_count);
}

TEST(UnitRnMan, Reset1)
{
        seed_seq_fe128 seseq{1, 2, 3, 4};
        pcg64          engine1(seseq);

        auto  seeds = generate_seed_vector<pcg64::state_type>(2, seseq);
        pcg64 engine2(seeds[1], seeds[0]);

        EXPECT_TRUE(engine1 == engine2);
}

TEST(UnitRnMan, Reset2)
{
        const RnPcg::Info info("1,2,3,4", "", 2);
        RnPcg             rnPcg(info);
        const auto        info2 = rnPcg.GetInfo();
        RnPcg             rnPcg2(info2);
        const auto        info3 = rnPcg2.GetInfo();

        EXPECT_EQ(info2.m_state, info3.m_state);
}

TEST(UnitRnMan, Distribution)
{
        const RnPcg::Info info("1,2,3,4", "", 2);

        RnPcg                            rnPcg1(info);
        std::normal_distribution<double> dist;
        auto                             gen1 = rnPcg1[0].variate_generator(dist);

        RnPcg rnPcg2(info);
        auto  gen2 = rnPcg2[0].variate_generator<double, std::normal_distribution>();

        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
}

TEST(UnitRnMan, Uniform1)
{
        const RnPcg::Info info("1,2,3,4", "", 2);

        RnPcg                                  rnPcg1(info);
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        auto                                   gen1 = rnPcg1[0].variate_generator(dist);

        RnPcg rnPcg2(info);
        auto  gen2 = rnPcg2[0].uniform_generator(0.0, 1.0);

        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
}

TEST(UnitRnMan, Uniform2)
{
        const RnPcg::Info info("1,2,3,4", "", 2);

        RnPcg                                       rnPcg1(info);
        std::uniform_int_distribution<unsigned int> dist(0, 10);
        auto                                        gen1 = rnPcg1[0].variate_generator(dist);

        RnPcg rnPcg2(info);
        auto  gen2 = rnPcg2[0].uniform_generator(0, 10);

        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
}

} // namespace Tests
