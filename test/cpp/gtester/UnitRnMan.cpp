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

#include "util/Rn.h"
#include "util/RnMan.h"
#include "util/StringUtils.h"

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>

using namespace std;
using namespace ::testing;
using namespace randutils;
using namespace stride::util;

namespace stride {
namespace util {

class RnPcg64 : public Rn<pcg64>
{
        using Rn<pcg64>::Rn;
};

} // namespace util
} // namespace stride

namespace Tests {
using pcg_extras::operator<<;

template <typename T>
class UnitRnMan : public ::testing::Test
{
};

TYPED_TEST_CASE_P(UnitRnMan);

TYPED_TEST_P(UnitRnMan, DefaultInfo)
{
        const RnInfo info;

        EXPECT_EQ("1,2,3,4", info.m_seed_seq_init);
        EXPECT_EQ("", info.m_state);
        EXPECT_EQ(1u, info.m_stream_count);
}

TYPED_TEST_P(UnitRnMan, Reset1)
{
        const RnInfo info("1,2,3,4", "", 1);
        TypeParam    rn(info);
        const auto   info2 = rn.GetInfo();
        TypeParam    rn2(info2);
        const auto   info3 = rn2.GetInfo();
        EXPECT_EQ(info2.m_state, info3.m_state);
}

TYPED_TEST_P(UnitRnMan, Reset2)
{
        const RnInfo info("1,2,3,4", "", 1);
        TypeParam    rn(info);
        const auto   info2 = rn.GetInfo();
        TypeParam    rn2(info2);
        const auto   info3 = rn2.GetInfo();
        EXPECT_EQ(info2.m_state, info3.m_state);
}

TYPED_TEST_P(UnitRnMan, Distribution)
{
        const RnInfo info("1,2,3,4", "", 2);

        TypeParam                        rnPcg1(info);
        std::normal_distribution<double> dist;
        auto                             gen1 = rnPcg1[0].variate_generator(dist);

        TypeParam rnPcg2(info);
        auto      gen2 = rnPcg2[0].template variate_generator<double, std::normal_distribution>();

        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
}

TYPED_TEST_P(UnitRnMan, Uniform1)
{
        const RnInfo info("1,2,3,4", "", 2);

        TypeParam                              rnPcg1(info);
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        auto                                   gen1 = rnPcg1[0].variate_generator(dist);

        TypeParam rnPcg2(info);
        auto      gen2 = rnPcg2[0].uniform_generator(0.0, 1.0);

        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
}

TYPED_TEST_P(UnitRnMan, Uniform2)
{
        const RnInfo info("1,2,3,4", "", 2);

        TypeParam                                   rnPcg1(info);
        std::uniform_int_distribution<unsigned int> dist(0, 10);
        auto                                        gen1 = rnPcg1[0].variate_generator(dist);

        TypeParam rnPcg2(info);
        auto      gen2 = rnPcg2[0].uniform_generator(0, 10);

        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
        EXPECT_DOUBLE_EQ(gen1(), gen2());
        EXPECT_TRUE(rnPcg1[0].engine() == rnPcg2[0].engine());
}

REGISTER_TYPED_TEST_CASE_P(UnitRnMan, DefaultInfo, Reset1, Reset2, Distribution, Uniform1, Uniform2);

// typedef ::testing::Types<RnPcg64, RnLcg64> TestedTypes;
typedef ::testing::Types<RnPcg64> TestedTypes;
INSTANTIATE_TYPED_TEST_CASE_P(Rn, UnitRnMan, TestedTypes);

} // namespace Tests
