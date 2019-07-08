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

#include "DeliveryMan.h"
#include "class_delivery.h"
#include "flex_delivery.h"
#include "myhayai/BenchControlHelper.h"
#include "myhayai/BenchmarkRunner.h"
#include "myhayai/PtreeViewer.h"
#include "myhayai/Test.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <chrono>
#include <exception>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace ::testing;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;
using namespace stride::util;
using namespace myhayai;

namespace Tests {

class MyhayaiTests : public ::testing::Test
{
public:
        /// TestCase set up.
        static void SetUpTestCase()
        {
                ClassDelivery();
                FlexDelivery();
                BenchmarkRunner&   runner = BenchmarkRunner::Instance();
                BenchControlHelper helper(runner.GetTestDescriptors(), vector<string>(), vector<string>({"Deluxe"}));
                auto               pv = make_shared<PtreeViewer<chrono::milliseconds>>();
                runner.Register(pv, bind(&PtreeViewer<chrono::milliseconds>::Update, pv, placeholders::_1));
                runner.RunTests(helper.GetIncludedNames());
                g_benchPt = pv->CGet();
                write_xml("tests/MyhayaiTest.xml", g_benchPt, std::locale(),
                          xml_writer_make_settings<ptree::key_type>(' ', 8));
        }

        /// Tearing down TestCase
        static void TearDownTestCase() {}

        static ptree g_benchPt;

protected:
        /// Destructor has to be virtual.
        ~MyhayaiTests() override = default;

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

ptree MyhayaiTests::g_benchPt;

TEST_F(MyhayaiTests, Run)
{
        EXPECT_EQ(9, g_benchPt.get<int>("benchmark.executed"));
        EXPECT_EQ(1, g_benchPt.get<int>("benchmark.disabled"));
        EXPECT_EQ(1, g_benchPt.get<int>("benchmark.excluded"));
}

} // namespace Tests
