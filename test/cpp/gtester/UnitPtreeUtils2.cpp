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
#include <boost/property_tree/xml_parser.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/irange.hpp>
#include <array>
#include <deque>
#include <forward_list>
#include <gtest/gtest.h>
#include <iostream>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

using namespace std;
using namespace stride::util;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {
namespace tests {

template <typename T>
class UnitPtreeUtils2 : public ::testing::Test
{
public:
};

typedef ::testing::Types<vector<int>, deque<int>, forward_list<int>, list<int>> TestCombos;
TYPED_TEST_CASE(UnitPtreeUtils2, TestCombos);

// ------------------------------------------
// Actual tests
// ------------------------------------------

TYPED_TEST(UnitPtreeUtils2, toSequence)
{
        auto pt = RunConfigManager::FromString(UnitPtreeUtilsHelper::GetXml1());
        auto c  = ToSequence<TypeParam>(pt.get_child("run"));

        EXPECT_EQ(true, boost::range::equal(boost::irange(0, 5), c));
}

} // namespace tests
} // namespace stride
