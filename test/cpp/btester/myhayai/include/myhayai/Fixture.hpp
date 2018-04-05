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
 * Header file for Fixture.
 */

#include "myhayai/Clock.hpp"
#include "myhayai/TestResult.hpp"

#include <cstddef>

namespace myhayai {

/// Base test class.
/// @ref SetUp is invoked before each run, and @ref TearDown is invoked
/// once the run is finished. Iterations rely on the same fixture
/// for every run.
/// The default test class does not contain any actual code in the
/// SetUp and TearDown methods, which means that tests can inherit
/// this class directly for non-fixture based benchmarking tests.
class Fixture
{
public:
        /// Set up the testing fixture for execution of a run.
        virtual void SetUp() {}

        /// Tear down the previously set up testing fixture after the execution run.
        virtual void TearDown() {}

        /// Run the test.
        /// @returns the number of nanoseconds the run took.
        uint64_t Run()
        {
                SetUp();
                Clock::TimePoint startTime = Clock::Now();
                TestBody();
                Clock::TimePoint endTime = Clock::Now();
                TearDown();
                return Clock::Duration(startTime, endTime);
        }

        ///
        virtual ~Fixture() = default;

protected:
        /// Test body.
        virtual void TestBody() {}
};

} // namespace myhayai
