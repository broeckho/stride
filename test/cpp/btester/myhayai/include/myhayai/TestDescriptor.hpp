#pragma once
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
 * Header file for TestDescriptor.
 */

#include "TestFactory.hpp"
#include "TestParametersDescriptor.hpp"

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

namespace myhayai {

/// Test descriptor.
struct TestDescriptor
{
        /// Initialize a new test descriptor.
        /// @param fixtureName Name of the fixture.
        /// @param testName Name of the test.
        /// @param runs Number of runs for the test.
        /// @param iterations Number of iterations per run.
        /// @param testFactory Test factory implementation for the test.
        /// @param parameters Parametrized test parameters.
        TestDescriptor(const char* fixtureName, const char* testName, std::size_t runs, TestFactory* testFactory,
                       TestParametersDescriptor parameters, bool isDisabled = false)
            : FixtureName(fixtureName), TestName(testName), CanonicalName(std::string(fixtureName) + "." + testName),
              Runs(runs), Factory(testFactory), Parameters(parameters), IsDisabled(isDisabled)
        {
        }

        /// Dispose of a test descriptor.
        ~TestDescriptor() { delete this->Factory; }

        std::string              FixtureName;   ///< Fixture name.
        std::string              TestName;      ///< Test name.
        std::string              CanonicalName; ///< Canonical name: <FixtureName>.<TestName>.
        std::size_t              Runs;          ///< Test runs.
        TestFactory*             Factory;       ///< Test factory.
        TestParametersDescriptor Parameters;    ///< Parameters for parametrized tests
        bool                     IsDisabled;    ///< Disabled.
};

} // namespace myhayai
