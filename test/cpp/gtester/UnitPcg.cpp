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
 * Implementation of pcg tests.
 */

#include <gtest/gtest.h>
#include <iostream>
#include <pcg/pcg_random.hpp>
#include <pcg/randutils.hpp>
#include <sstream>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace randutils;

TEST(UnitPcg, Reset1)
{
        seed_seq_fe128 seseq{1, 2, 3, 4};
        pcg64          engine1(seseq); // seeded engine

        stringstream ss1;
        ss1 << engine1; // capture initial engine state

        engine1(); // advance engine one step

        ss1 >> engine1; // reset to initial state

        EXPECT_TRUE(engine1 == pcg64(seseq));
}

TEST(UnitPcg, Reset1bis)
{
        seed_seq_fe128 seseq{1, 2, 3, 4};
        pcg64          engine1(seseq); // seeded engine

        stringstream ss1;
        ss1 << engine1; // capture initial engine state
        auto s1 = ss1.str();
        ss1 << "a"; // stick a non-digit at the end of the stream

        engine1(); // advance engine one step

        ss1 >> engine1; // reset to initial state

        EXPECT_TRUE(engine1 == pcg64(seseq));
}

TEST(UnitPcg, SeparatorRequired)
{
        seed_seq_fe128 seseq{1, 2, 3, 4};
        pcg64          engine1(seseq); // seeded engine

        stringstream ss1;
        ss1 << engine1; // capture initial engine state
        auto s1 = ss1.str();
        ss1 << "1"; // stick a digit at the end of stream

        engine1(); // advance engine one step

        ss1 >> engine1; // reset to initial state

        EXPECT_FALSE(engine1 == pcg64(seseq));
}

TEST(UnitPcg, Reset2)
{
        seed_seq_fe128 seseq{1, 2, 3, 4};
        pcg64          engine1(seseq); // seeded engine
        pcg64          engine2;        // default constructed engine

        stringstream ss1;
        ss1 << engine1; // capture initial seeded state
        ss1 >> engine2; // insert seeded state into default engine

        EXPECT_TRUE(engine1 == engine2);
}

TEST(UnitPcg, MultipleReset)
{
        seed_seq_fe128 seseq{1, 2, 3, 4};
        pcg64          engine1(seseq); // seeded engine
        pcg64          engine2;        // default constructed engine

        stringstream ss;
        ss << engine1; // capture state seeded engine
        ss << " ";     // need to insert a separator
        ss << engine2; // capture state default engine

        engine1(); // advance engine1
        engine2(); // advance engine2

        ss >> engine1; // reset to initial state
        ss >> engine2; // reset to initial state;

        EXPECT_TRUE(engine1 == pcg64(seseq));
        EXPECT_TRUE(engine2 == pcg64());
}

} // namespace Tests
