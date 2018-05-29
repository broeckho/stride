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
 * Unit tests of PtreeUtils.
 */

#include "UnitPtreeUtilsHelper.h"
#include "util/PtreeUtils.h"
#include "util/RunConfigManager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/irange.hpp>
#include <array>
#include <deque>
#include <forward_list>
#include <gtest/gtest.h>
#include <iostream>
#include <list>
#include <vector>

using namespace std;
using namespace stride::util;
using namespace boost::property_tree;

namespace stride {
namespace tests {

class UnitPtreeUtils1 : public ::testing::Test
{
public:
};

TEST(UnitPtreeUtils1, toArray)
{
        auto pt = RunConfigManager::FromString(UnitPtreeUtilsHelper::GetXml1());
        auto l  = ToArray<int, 5>(pt.get_child("run"));

        EXPECT_EQ(true, boost::range::equal(boost::irange(0, 5), l));
}

TEST(UnitPtreeUtils1, Merge)
{
        auto pt1 = RunConfigManager::FromString(UnitPtreeUtilsHelper::GetXml1());
        auto pt2 = RunConfigManager::FromString(UnitPtreeUtilsHelper::GetXml2());
        auto pt  = Merge(pt1.get_child("run"), pt2.get_child("run"));
        auto c   = ToSequence<list<int>>(pt);

        EXPECT_EQ(true, boost::range::equal(boost::irange(0, 9), c));
}

} // namespace tests
} // namespace stride
