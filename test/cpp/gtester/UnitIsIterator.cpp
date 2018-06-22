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
 * Unit tests of Csv.
 */

#include "util/SegmentedVector.h"
#include "util/is_iterator.h"

#include <gtest/gtest.h>
#include <vector>

using namespace std;
using namespace stride::util;

TEST(UnitIsIterator, Vector)
{
        EXPECT_TRUE(is_iterator<vector<int>::iterator>::value);
        EXPECT_TRUE(is_iterator<vector<int>::const_iterator>::value);
}

TEST(UnitIsIterator, SegVec)
{
        EXPECT_TRUE(is_iterator<SegmentedVector<int>::iterator>::value);
        EXPECT_TRUE(is_iterator<SegmentedVector<int>::const_iterator>::value);
}

TEST(UnitIsIterator, Pointer)
{
        EXPECT_TRUE(is_iterator<double*>::value);
        EXPECT_TRUE(is_iterator<const double*>::value);
}

TEST(UnitIsIterator, Double)
{
        EXPECT_FALSE(is_iterator<double>::value);
        EXPECT_FALSE(is_iterator<const double>::value);
}
