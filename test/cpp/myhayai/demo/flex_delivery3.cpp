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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */
/**
 * @file
 * Header file for TestFactory.
 */

#include "DeliveryMan.h"
#include "SlowDeliveryMan.h"
#include "myhayai/Benchmark.hpp"
#include "myhayai/BenchmarkRunner.hpp"
#include "myhayai/Test.hpp"
#include "util/StringUtils.h"

#include <memory>
#include <thread>

using namespace std;
using namespace stride::util;
using namespace myhayai;

// Now we're cooking, with another indirection layer. the factory_builder builds test_factories.
// It can accept parameters (which the factory cannot since it has to have a void() signature)
// At registration time, the call to the factory builder the produces the factory.
auto param_factory_builder = [](unsigned int distance, unsigned int duration, unsigned int speed) { //
        return [distance, duration, speed]() {                                                      //
                autop                     = make_shared<DeliveryMan>();                                            //
                return Test(                                                                        //
                    [p, duration, distance]() {                                                     //
                            this_thread::sleep_for(duration * 10ms);                                //
                            p->DeliverPackage(distance);                                            //
                    },                                                                              //
                    [p, speed]() { *p = DeliveryMan(speed); }                                       //
                );                                                                                  //
        };                                                                                          //
};                                                                                                  //

bool register_many()
{
        for (unsigned int i = 1; i < 4; ++i) {
                BenchmarkRunner::RegisterTest("FlexDelivery", "Flex3 - " + ToString(i), 10,
                                              param_factory_builder(1, 1, i));
        }
        volatile auto somecondition = true;
        if (somecondition) {
                BenchmarkRunner::RegisterTest("FlexDelivery", "Flexconditional", 1, param_factory_builder(5, 80, 1));
        }
        return true;
}

// Again, just to trigger execution
namespace {
bool      b0 = register_many();
Benchmark b1("FlexDelivery", "Flex3 - distance=50, duration=80, speed=1", 1, param_factory_builder(50, 80, 1));
Benchmark b2("FlexDelivery", "Flex3 - distance=1, duration=1, speed=1", 10, param_factory_builder(1, 1, 1));
} // namespace
