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
#include "Param2TestFactory.h"
#include "SlowDeliveryMan.h"
#include "myhayai/Benchmark.hpp"
#include "myhayai/MainRunner.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace myhayai;
using namespace std::chrono_literals;

auto param2_factory = []() {
        auto p = make_shared<DeliveryMan>();
        return Fixture(
            [p]() {
                    this_thread::sleep_for(10ms);
                    p->DeliverPackage(5);
            },
            [p]() { *p = DeliveryMan(3); });
};

namespace {
Benchmark b("FlexDelivery", "Flex2 - distance=5, speed=3", 10, param2_factory);
}
