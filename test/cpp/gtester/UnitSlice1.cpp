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
 *  Copyright 2017, 2018 Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Unit tests of Slice & SegmentedVector.
 */

#include "util/SegmentedVector.h"
#include "util/SliceIndexer.h"

#include <boost/range/irange.hpp>
#include <array>
#include <gtest/gtest.h>
#include <iterator>
#include <type_traits>

using namespace std;
using namespace stride::util;

namespace stride {
namespace tests {

// ------------------------------------------
// General fixture, used to define container.
template <typename T>
class UnitSliceIndex1 : public ::testing::Test
{
public:
        static T c;
};
template <typename T>
T UnitSliceIndex1<T>::c(101);

// ------------------------------------------
// Fixture specialisation for std::array, used to define container.
template <>
class UnitSliceIndex1<array<int, 101>> : public ::testing::Test
{
public:
        static array<int, 101> c;
};
array<int, 101> UnitSliceIndex1<array<int, 101>>::c;

// ------------------------------------------
// Fixture specialisation for built-in array, used to define container.
template <>
class UnitSliceIndex1<int[101]> : public ::testing::Test
{
public:
        static int c[101];
};
int UnitSliceIndex1<int[101]>::c[101];

// ------------------------------------------
// Following googletest's prescriptions for typed tests.
typedef ::testing::Types<vector<int>, array<int, 101>, int[101], SegmentedVector<int, 4, false>> TestCombos;
TYPED_TEST_CASE(UnitSliceIndex1, TestCombos);

// ------------------------------------------
// Actual tests
// ------------------------------------------

TYPED_TEST(UnitSliceIndex1, Setup)
{
        auto& c = TestFixture::c;
        boost::range::copy(boost::irange(0, 101), begin(c));
        EXPECT_EQ(0, c[0]);
        EXPECT_EQ(1, c[1]);
        EXPECT_EQ(50, c[50]);
        EXPECT_EQ(100, c[100]);
}

TYPED_TEST(UnitSliceIndex1, Size1)
{
        auto& c = TestFixture::c;
        boost::range::copy(boost::irange(0, 101), begin(c));

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, 101, "last_71");

        const auto& rvec = si.Get();
        EXPECT_EQ(10, rvec[0].size());
        EXPECT_EQ(20, rvec[1].size());
        EXPECT_EQ(71, rvec[2].size());
}

TYPED_TEST(UnitSliceIndex1, Size2)
{
        auto& c = TestFixture::c;
        boost::range::copy(boost::irange(0, 101), begin(c));

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, 101, "last_71");

        EXPECT_EQ(10, si.Get("first_10").size());
        EXPECT_EQ(20, si.Get("next_20").size());
        EXPECT_EQ(71, si.Get("last_71").size());
}

TYPED_TEST(UnitSliceIndex1, Content1)
{
        auto& c = TestFixture::c;
        boost::range::copy(boost::irange(0, 101), begin(c));

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, 101, "last_71");

        const auto& rvec = si.Get();
        EXPECT_EQ(0, rvec[0][0]);
        EXPECT_EQ(9, rvec[0][9]);
        EXPECT_EQ(10, rvec[1][0]);
        EXPECT_EQ(29, rvec[1][19]);
        EXPECT_EQ(30, rvec[2][0]);
        EXPECT_EQ(100, rvec[2][70]);
}

TYPED_TEST(UnitSliceIndex1, Content2)
{
        auto& c = TestFixture::c;
        boost::range::copy(boost::irange(0, 101), begin(c));

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");
        const auto& rvec = si.Get();

        int sub1 = 30;
        for (auto e : rvec[2]) {
                EXPECT_EQ(sub1, e);
                sub1++;
        }

        int accum = 0;
        for (auto e : rvec[0]) {
                accum += e;
        }
        EXPECT_EQ(45, accum);

        auto s1 = si.Get("next_20");
        for (size_t i = 0; i < s1.size(); ++i) {
                EXPECT_EQ(i + 10, s1[i]);
        }

        auto s2   = si.Get("last_71");
        int  sub2 = 30;
        for (auto it = s2.begin(); it < s2.end(); ++it) {
                EXPECT_EQ(sub2, *it);
                sub2++;
        }
}

} // namespace tests
} // namespace stride
