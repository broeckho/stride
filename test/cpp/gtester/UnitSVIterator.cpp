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
 * Unit tests of SVIterator & SegmentedVector.
 */

#include "util/SegmentedVector.h"

#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>
#include <gtest/gtest.h>

using namespace std;
using namespace stride::util;

namespace stride {
namespace tests {

class Base
{
public:
        virtual size_t Get1() const { return 0; }
        virtual ~Base() = default;
};

class Derived : public Base
{
public:
        size_t         Get1() const override { return 1; }
        size_t         Get2() const { return 2; }
        virtual size_t Get3() const { return 3; }
};

TEST(UnitSVIterator, Size)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());
        EXPECT_EQ(c.end() - c.begin(), c.size());
        EXPECT_EQ(c.begin() + c.size(), c.end());
        EXPECT_EQ(c.end() - c.size(), c.begin());
}

TEST(UnitSVIterator, Resize)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());
        EXPECT_EQ(c.end() - c.begin(), c.size());
        EXPECT_EQ(c.begin() + c.size(), c.end());
        EXPECT_EQ(c.end() - c.size(), c.begin());

        c.resize(199);
        EXPECT_EQ(c.end() - c.begin(), c.size());
        EXPECT_EQ(c.begin() + c.size(), c.end());
        EXPECT_EQ(c.end() - c.size(), c.begin());

        c.resize(25);
        EXPECT_EQ(c.end() - c.begin(), c.size());
        EXPECT_EQ(c.begin() + c.size(), c.end());
        EXPECT_EQ(c.end() - c.size(), c.begin());
}

TEST(UnitSVIterator, Loop1)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        int i = 0;
        for (auto it = c.begin(); it < c.end(); ++it) {
                EXPECT_EQ(i, *it);
                i++;
        }
}

TEST(UnitSVIterator, Loop2)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        int i = 0;
        for (auto e : c) {
                EXPECT_EQ(i, e);
                i++;
        }
}

TEST(UnitSVIterator, Loop3)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto it = c.begin();
        for (size_t i = 0; i < c.size(); ++i) {
                EXPECT_EQ(i, *it);
                ++it;
        }
        EXPECT_EQ(c.end(), it);
}

TEST(UnitSVIterator, End)
{
        SegmentedVector<int, 4, false> c(5);
        boost::range::copy(boost::irange(0, static_cast<int>(c.size())), c.begin());

        auto it = c.end();
        for (size_t i = 0; i < 5; ++i) {
                --it;
        }
        EXPECT_EQ(c.begin(), it);
}

TEST(UnitSVIterator, Direct1)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto it = c.begin();
        for (size_t i = 0; i < 5; ++i) {
                ++it;
        }
        EXPECT_EQ(c.begin() + 5, it);
        EXPECT_EQ(5 + c.begin(), it);
        auto it5 = it;
        for (size_t i = 0; i < 3; ++i) {
                --it;
        }
        EXPECT_EQ(it5 - 3, it);
        EXPECT_EQ(-3 + it5, it);
        EXPECT_EQ(c.begin() + 2, it);
}

} // namespace tests
} // namespace stride
