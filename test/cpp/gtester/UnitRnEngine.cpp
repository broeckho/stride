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

#include <trng/lcg64.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <pcg/pcg_random.hpp>
#include <randutils/randutils.hpp>
#include <sstream>

namespace Tests {

using namespace std;
using namespace ::testing;
using namespace randutils;

template <typename T>
class UnitRnEngine : public ::testing::Test
{
};

TYPED_TEST_CASE_P(UnitRnEngine);

TYPED_TEST_P(UnitRnEngine, Reset1)
{
        TypeParam    engine1(517UL); // seeded engine
        stringstream ss1;
        ss1 << engine1; // capture initial engine state

        engine1();      // advance engine one step
        ss1 >> engine1; // reset to initial state

        EXPECT_TRUE(engine1 == TypeParam(517UL));
}

TYPED_TEST_P(UnitRnEngine, Reset1bis)
{
        TypeParam    engine1(517UL); // seeded engine
        stringstream ss1;
        ss1 << engine1; // capture initial engine state
        ss1 << "a";     // stick a non-digit at the end of the stream

        engine1();      // advance engine one step
        ss1 >> engine1; // reset to initial state

        EXPECT_TRUE(engine1 == TypeParam(517UL));
}

TYPED_TEST_P(UnitRnEngine, SeparatorNotRequired)
{
        TypeParam    engine1(517UL); // seeded engine
        stringstream ss1;
        ss1 << engine1; // capture initial engine state
        ss1 << "1";     // stick a digit at the end of stream

        engine1();      // advance engine one step
        ss1 >> engine1; // reset to initial state

        EXPECT_TRUE(engine1 == TypeParam(517UL));
}

TYPED_TEST_P(UnitRnEngine, Reset2)
{
        TypeParam    engine1(517UL); // seeded engine
        TypeParam    engine2;        // default constructed engine
        stringstream ss1;
        ss1 << engine1; // capture initial seeded state
        ss1 >> engine2; // insert seeded state into default engine

        EXPECT_TRUE(engine1 == engine2);
}

TYPED_TEST_P(UnitRnEngine, MultipleReset)
{
        TypeParam    engine1(517UL); // seeded engine
        TypeParam    engine2;        // default constructed engine
        stringstream ss;
        ss << engine1; // capture state seeded engine
        ss << " ";     // need to insert a separator
        ss << engine2; // capture state default engine

        engine1(); // advance engine1
        engine2(); // advance engine2

        ss >> engine1; // reset to initial state
        ss >> engine2; // reset to initial state;

        EXPECT_TRUE(engine1 == TypeParam(517UL));
        EXPECT_TRUE(engine2 == TypeParam());
}

REGISTER_TYPED_TEST_CASE_P(UnitRnEngine, Reset1, Reset1bis, SeparatorNotRequired, Reset2, MultipleReset);

typedef ::testing::Types<pcg64, trng::lcg64> TestedTypes;
INSTANTIATE_TYPED_TEST_CASE_P(Engine, UnitRnEngine, TestedTypes);

} // namespace Tests
