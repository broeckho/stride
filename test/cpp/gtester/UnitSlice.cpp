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
#include <gtest/gtest.h>
#include <type_traits>

using namespace std;
using namespace stride::util;

namespace SimPT_Sim {
namespace Container {
namespace Tests {

template<typename T>
class UnitRangeIndex : public ::testing::Test
{
public:
};

typedef ::testing::Types<vector<int>, SegmentedVector<int, 4>, SegmentedVector<int, 4, false>> RandomAccessContainers;
TYPED_TEST_CASE(UnitRangeIndex, RandomAccessContainers);

TYPED_TEST(UnitRangeIndex, Setup)
{
        TypeParam c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());
        EXPECT_EQ(0, c[0]);
        EXPECT_EQ(1, c[1]);
        EXPECT_EQ(50, c[50]);
        EXPECT_EQ(100, c[100]);
}

TYPED_TEST(UnitRangeIndex, Size1)
{
        TypeParam c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        SliceIndexer<TypeParam> si(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, 101, "last_71");

        const auto& rvec = si.Get();
        EXPECT_EQ(10, rvec[0].size());
        EXPECT_EQ(20, rvec[1].size());
        EXPECT_EQ(71, rvec[2].size());
}

TYPED_TEST(UnitRangeIndex, Size2)
{
        TypeParam c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, 101, "last_71");

        EXPECT_EQ(10, si.Get("first_10").size());
        EXPECT_EQ(20, si.Get("next_20").size());
        EXPECT_EQ(71, si.Get("last_71").size());
}

TYPED_TEST(UnitRangeIndex, Content1)
{
        TypeParam c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

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

TYPED_TEST(UnitRangeIndex, Content2)
{
        TypeParam c(101, 0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");
        const auto& rvec = si.Get();

        int sub1 = 30;
        for (auto e : rvec[2]) {
                EXPECT_EQ(sub1++, e);
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
                EXPECT_EQ(sub2++, *it);
        }
}

TYPED_TEST(UnitRangeIndex, Resize1)
{
        TypeParam c(101, 0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");
        const auto& rvec = si.Get();

        if (!is_same<TypeParam, vector<int>>::value) {
                c.resize(201);

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
                        EXPECT_EQ(sub2++, *it);
                }
        }
}

TYPED_TEST(UnitRangeIndex, Resize2)
{
        TypeParam c(101, 0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");

        if (!is_same<TypeParam, vector<int>>::value) {
                c.resize(201);
                si.Set(101, "new_last");
                boost::range::copy(boost::irange(101, 201), si.Get("new_last").begin());
        }

}

TYPED_TEST(UnitRangeIndex, Resize3)
{
        TypeParam c(101, 0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");

        if (!is_same<TypeParam, vector<int>>::value) {
                c.resize(201);

                si.Set(101, "new_last");
                boost::range::copy(boost::irange(101, 201), si.Get("new_last").begin());

                int accum = 0;
                for (auto e : si.Get("first_10")) {
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
                        EXPECT_EQ(sub2++, *it);
                }

                int sub3 = 101;
                for (auto e : si.Get("new_last")) {
                        EXPECT_EQ(sub3++, e);
                }
        }
}

} // namespace Tests
} // namespace Container
} // namespace SimPT_Sim
