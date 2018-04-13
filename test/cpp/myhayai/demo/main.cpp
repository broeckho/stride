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
#include "myhayai/Test.hpp"
#include "myhayai/CliController.hpp"
#include "util/StringUtils.h"

#include <exception>
#include <iostream>
#include <thread>

using namespace std;
using namespace stride::util;
using namespace myhayai;

int main(int argc, char** argv)
{
        int exit_status = EXIT_SUCCESS;

        // Building the tests with lambdas makes it easy to define them in main (or some
        // function called directly by main prior to benchmarking) and do away the global
        // variables used for registration outside main.
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

        for (unsigned int i = 1; i < 4; ++i){
                BenchmarkRunner::RegisterTest("FlexDelivery", "FlexMain - " + ToString(i), 10,
                                              param_factory_builder(1, 1, i));
        }

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
