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
 */
/**
 * @file
 * Header file for Benchmark.
 */

#include "BenchmarkRunner.h"
#include "InfoFactory.h"
#include "TestFactory.h"

#include <utility>

namespace myhayai {

/**
 * This class allows you to register test via instantiation. It also allows you to
 * check success/failre of the registration (via the bool conversion operator).
 */
class Benchmark
{
public:
        /// Delegating constructor.
        Benchmark(const std::string& groupName, const std::string& testName, std::size_t numRuns,
                  TestFactory testFactory)
            : Benchmark(groupName, testName, numRuns, std::move(testFactory), InfoFactory(), false)
        {
        }

        /// Delegating Constructor.
        Benchmark(const std::string& groupName, const std::string& testName, std::size_t numRuns,
                  TestFactory testFactory, bool disableTest)
            : Benchmark(groupName, testName, numRuns, std::move(testFactory), InfoFactory(), disableTest)
        {
        }

        /// Constructor that registers test. For parameters @see TestDescriptor.
        Benchmark(const std::string& groupName, const std::string& testName, std::size_t numRuns,
                  TestFactory testFactory, InfoFactory infoFactory, bool disableTest = false)
        {
                m_status = BenchmarkRunner::RegisterTest(groupName, testName, numRuns, std::move(testFactory),
                                                         std::move(infoFactory), disableTest);
        }

        /// True iff succesfully registered.
        explicit operator bool() const { return m_status; }

private:
        bool m_status = false;
};

} // namespace myhayai
