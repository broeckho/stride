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

#include "BenchmarkRunner.hpp"
#include "InfoFactory.hpp"
#include "TestFactory.hpp"

#include <utility>

namespace myhayai {

class Benchmark
{
public:
        ///
        Benchmark(const std::string& groupName, const std::string& testName, std::size_t numRuns,
                  TestFactory testFactory)
            : Benchmark(groupName, testName, numRuns, std::move(testFactory), InfoFactory(), false)
        {
        }

        ///
        Benchmark(const std::string& groupName, const std::string& testName, std::size_t numRuns,
                  TestFactory testFactory, bool disableTest)
            : Benchmark(groupName, testName, numRuns, std::move(testFactory), InfoFactory(), disableTest)
        {
        }

        ///
        Benchmark(const std::string& groupName, const std::string& testName, std::size_t numRuns,
                  TestFactory testFactory, InfoFactory infoFactory, bool disableTest = false)
        {
                m_status = BenchmarkRunner::RegisterTest(groupName, testName, numRuns, std::move(testFactory),
                                                         std::move(infoFactory), disableTest);
        }

        explicit operator bool() const { return m_status; }

private:
        bool m_status = false;
};

} // namespace myhayai
