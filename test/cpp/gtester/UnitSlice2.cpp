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

namespace stride {
namespace tests {

template <typename T>
class UnitSliceIndex2 : public ::testing::Test
{
public:
};

typedef ::testing::Types<SegmentedVector<int, 4>, SegmentedVector<int, 4, false>> TestCombos;
TYPED_TEST_CASE(UnitSliceIndex2, TestCombos);

TYPED_TEST(UnitSliceIndex2, Resize1)
{
        TypeParam c(101, 0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");
        const auto& rvec = si.Get();

        c.resize(201);

        int accum = 0;
        for (auto e : si.Get("first_10")) {
                accum += e;
        }
        EXPECT_EQ(45, accum);

        auto s1 = rvec[1];
        for (size_t i = 0; i < s1.size(); ++i) {
                EXPECT_EQ(i + 10, s1[i]);
        }

        auto s2   = rvec[2];
        int  sub2 = 30;
        for (auto it = s2.begin(); it < s2.end(); ++it) {
                EXPECT_EQ(sub2, *it);
                sub2++;
        }
}

TYPED_TEST(UnitSliceIndex2, Resize2)
{
        TypeParam c(101, 0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");

        c.resize(201);
        si.Set(101, "new_last");
        boost::range::copy(boost::irange(101, 201), si.Get("new_last").begin());
}

TYPED_TEST(UnitSliceIndex2, Resize3)
{
        TypeParam c(101, 0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_slice_indexer(c);
        si.Set(0, 10, "first_10");
        si.Set(10, 30, "next_20");
        si.Set(30, "last_71");

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
                EXPECT_EQ(sub2, *it);
                sub2++;
        }

        int sub3 = 101;
        for (auto e : si.Get("new_last")) {
                EXPECT_EQ(sub3, e);
                sub3++;
        }
}

} // namespace tests
} // namespace stride
