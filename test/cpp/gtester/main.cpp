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
 *  Copyright 2017, Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Main program for test runs.
 */

#include "util/LogUtils.h"

#include <cerrno>
#include <exception>
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
        std::cout << "START TEST ENVIRONMENT" << std::endl;

        int exit_status = EXIT_SUCCESS;
        try {
                auto logger = stride::util::LogUtils::CreateFileLogger("gtester_logger", "gtester_logger.txt");
                spdlog::register_logger(logger);
                ::testing::InitGoogleTest(&argc, argv);
                return RUN_ALL_TESTS();
        } catch (std::exception& e) {
                cerr << "Exception caught: " << e.what() << endl << endl;
                exit_status = EXIT_FAILURE;
        }
        return exit_status;
}
