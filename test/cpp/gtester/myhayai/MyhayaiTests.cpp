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
#include <gtest/gtest.h>
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

using namespace std;
using namespace ::testing;
using boost::property_tree::ptree;
using namespace stride::util;
using namespace myhayai;

namespace Tests {

class MyhayaiTests : public ::testing::Test
{
public:
        /// TestCase set up.
        static void SetUpTestCase()
        {
                class_delivery();
                flex_delivery();
                // Building the tests with lambdas also makes it easy to define.
                // clang-format off
                auto param_factory_builder = [](unsigned int distance, unsigned int duration, unsigned int speed) {
                        return [distance, duration, speed]() {
                                auto p = make_shared<DeliveryMan>();
                                return myhayai::Test(
                                        [p, duration, distance]() {
                                                this_thread::sleep_for(duration * 100ms);
                                                p->DeliverPackage(distance);
                                        },
                                        [p, speed]() { *p = DeliveryMan(speed); });
                        };
                };
                // clang-format on

                for (unsigned int i = 1; i < 4; ++i) {
                        BenchmarkRunner::RegisterTest("FlexDelivery", "FlexMain - " + ToString(i), 10,
                                                      param_factory_builder(10, 1, i));
                }

                BenchmarkRunner&   runner = BenchmarkRunner::Instance();
                const vector<string> negative;
                const vector<string> positive;
                BenchControlHelper helper(runner.GetTestDescriptors(), negative, positive);
                auto pv = make_shared<PtreeViewer<chrono::milliseconds>>();
                runner.Register(pv, bind(&PtreeViewer<chrono::milliseconds>::Update, pv, placeholders::_1));
                runner.RunTests(helper.GetIncludedNames());
        }

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~MyhayaiTests() override = default;

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST(MyhayaiTests, Run)
{

}

} // namespace Tests
