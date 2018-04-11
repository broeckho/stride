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

auto param2_factory_builder = [](unsigned int distance, unsigned int duration, unsigned int speed) {
        return [distance, duration, speed]() {
                auto p = make_shared<DeliveryMan>();
                return Test(
                    [p, duration, distance]() {
                            this_thread::sleep_for(duration * 10ms);
                            p->DeliverPackage(distance);
                    },
                    [p, speed]() { *p = DeliveryMan(speed); });
        };
};

namespace {

Benchmark b1("FlexDelivery", "Flex3 - distance=50, duration=80, speed=1", 1, param2_factory_builder(50, 80, 1));
Benchmark b2("FlexDelivery", "Flex3 - distance=1, duration=1, speed=1", 10, param2_factory_builder(1, 1, 1));
} // namespace
