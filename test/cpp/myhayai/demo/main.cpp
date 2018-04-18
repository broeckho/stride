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
 *  This software has been (completely) rewritten starting from
 *  the hayai code by Nick Bruun. The original copyright, to be
 *  found in this directory still aplies.
 */
/**
 * @file
 * Impleme.
 */

#include "DeliveryMan.h"
#include "myhayai/BenchmarkRunner.hpp"
#include "myhayai/CliController.hpp"
#include "myhayai/Test.hpp"
#include "util/StringUtils.h"

#include <exception>
#include <iostream>
#include <thread>

using namespace std;
using namespace stride::util;
using namespace myhayai;

void class_delivery();
void flex_delivery();

int main(int argc, char** argv)
{
        int exit_status = EXIT_SUCCESS;

        // Including tests registered elsewhere in some manner is necessary. You can do that
        // as I did, by packaging them in a function and call it here. Or you can introduce
        // variables with static storage duration outside of main, whose initialization
        // triggers the registration of the test. The Benchmark clas is designed to enable
        // just that. I prefer the former approach though..
        class_delivery();
        flex_delivery();

        // Building the tests with lambdas also makes it easy to define them in main itself.
        // clang-format off
        auto param_factory_builder = [](unsigned int distance, unsigned int duration, unsigned int speed) {
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
        // clang-format on

        for (unsigned int i = 1; i < 4; ++i) {
                BenchmarkRunner::RegisterTest("FlexDelivery", "FlexMain - " + ToString(i), 10,
                                              param_factory_builder(1, 1, i));
        }

        // This is the actual main.
        try {
                CliController controller;
                controller.ParseArgs(argc, argv);
                controller.Control();

        } catch (exception& e) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }

        return exit_status;
}
