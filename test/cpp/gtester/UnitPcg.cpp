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
#include <pcg/pcg_random.hpp>
#include <pcg/randutils.hpp>
#include <chrono>
#include <exception>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;
using namespace ::testing;
using namespace randutils;
using namespace stride::util;

namespace Tests {

class UnitPcg : public ::testing::Test
{
public:
        /// TestCase set up.
        static void SetUpTestCase(){}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~UnitPcg() override = default;

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST_F(UnitPcg, DefaultInfo)
{
        const RnPcg::Info info;

        EXPECT_EQ("1,2,3,4", info.m_seed_seq_init);
        EXPECT_EQ("", info.m_state);
        EXPECT_EQ(1u, info.m_stream_count);
}

TEST_F(UnitPcg, GetInfo1)
{
        RnPcg rnPcg;
        const auto info = rnPcg.GetInfo();

        EXPECT_EQ("1,2,3,4", info.m_seed_seq_init);
        EXPECT_EQ(1u, info.m_stream_count);
}

TEST_F(UnitPcg, Reset1)
{
        seed_seq_fe128 seseq{1,2,3,4};

        pcg64 engine1(seseq);
        stringstream ss1;
        ss1 << engine1;        // capture initial engine state

        engine1();             // advance engine one step

        ss1 >> engine1;        // insert initial state back into engine
        stringstream ss2;
        ss2 << engine1;        // capture final state

        EXPECT_TRUE(ss1.str() == ss2.str());
}

TEST_F(UnitPcg, Reset1bis)
{
        seed_seq_fe128 seseq{1,2,3,4};

        pcg64 engine1(seseq);
        stringstream ss1;
        ss1 << engine1;        // capture initial engine state
        auto s1 = ss1.str();
        ss1 << "a";            // stick a non-digit at the end

        engine1();             // advance engine one step

        ss1 >> engine1;        // insert initial state back into engine
        stringstream ss2;
        ss2 << engine1;        // capture final state

        EXPECT_TRUE(s1 == ss2.str());
}


TEST_F(UnitPcg, Reset2)
{
        seed_seq_fe128 seseq{1,2,3,4};

        pcg64 engine1(seseq);
        stringstream ss1;
        ss1 << engine1;        // capture initial engine state

        pcg64 engine2;         // default constructed engine

        ss1 >> engine2;        // insert initial state into default engine
        stringstream ss2;
        ss2 << engine2;        // capture final state

        EXPECT_TRUE(engine1 == engine2);
        EXPECT_TRUE(ss1.str() == ss2.str());
}


} // namespace Tests
