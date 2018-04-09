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
 *  The original copyright, to be found in the directory one level higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for Benchmarker.
 */

#include "myhayai/Benchmarker.hpp"

#include "myhayai/ConsoleOutputter.hpp"
#include "myhayai/Fixture.hpp"
#include "myhayai/InfoFactory.hpp"
#include "myhayai/TestDescriptors.hpp"
#include "myhayai/TestFactory.hpp"
#include "myhayai/TestResult.hpp"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace myhayai {

size_t Benchmarker::GetDisabledCount(const vector<string>& names)
{
        size_t disabledCount = 0;
        for (const auto& n : names) {
                if (m_test_descriptors[n].m_is_disabled) {
                        ++disabledCount;
                }
        }
        return disabledCount;
}

const TestDescriptors& Benchmarker::GetTestDescriptors() const { return m_test_descriptors; }

Benchmarker& Benchmarker::Instance()
{
        static Benchmarker singleton;
        return singleton;
}

bool Benchmarker::RegisterTest(const char* fixtureName, const char* testName, size_t numRuns, TestFactory testFactory,
                               InfoFactory infoFactory, bool isDisabled)
{
        TestDescriptor d(fixtureName, testName, numRuns, std::move(testFactory), std::move(infoFactory), isDisabled);
        const auto     ret = Instance().m_test_descriptors.emplace(make_pair(d.GetCanonicalName(), d));
        return ret.second;
}

void Benchmarker::RunTests(const vector<string>& names)
{
        ConsoleOutputter defaultOutputter;

        // Setup.
        auto         test_descriptors = Instance().GetTestDescriptors();
        const size_t totalCount       = test_descriptors.size();
        const size_t disabledCount    = Instance().GetDisabledCount(names);
        const size_t enabledCount     = totalCount - disabledCount;

        // Begin output.
        defaultOutputter.Begin(enabledCount, disabledCount);

        // Run through all the test_descriptors in ascending order.
        for (const auto& n : names) {

                const auto& t_d = test_descriptors[n];

                // If test is disabled output and skip.
                if (t_d.m_is_disabled) {
                        defaultOutputter.SkipDisabledTest(t_d.m_fixture_name, t_d.m_test_name, t_d.m_info_factory,
                                                          t_d.m_num_runs);
                        continue;
                }

                // Describe the beginning of the run.
                defaultOutputter.BeginTest(t_d.m_fixture_name, t_d.m_test_name, t_d.m_info_factory, t_d.m_num_runs);

                // Execute each individual run.
                vector<uint64_t> run_times(t_d.m_num_runs);
                for (auto& rt : run_times) {
                        Fixture test = t_d.m_test_factory();
                        rt           = test.Run();
                }

                // Calculate the test result.
                TestResult results(run_times);

                // Describe the end of the run.
                defaultOutputter.EndTest(t_d.m_fixture_name, t_d.m_test_name, t_d.m_info_factory, results);
        }

        // End output.
        defaultOutputter.End(enabledCount, disabledCount);
}

} // namespace myhayai
