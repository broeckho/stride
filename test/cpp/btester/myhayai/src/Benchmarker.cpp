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

#include "myhayai/Fixture.hpp"
#include "myhayai/Id.hpp"
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
        // Setup.
        auto test_descriptors = Instance().GetTestDescriptors();
        Notify(event::Payload(event::Id::BeginBench));

        // Run through all the test_descriptors in ascending order.
        for (const auto& n : names) {

                const auto& t_d = test_descriptors[n];

                // If test is disabled, then skip it.
                if (t_d.m_is_disabled) {
                        Notify(event::Payload(event::Id::SkipTest, n));
                        continue;
                }

                // Runs for test with canonical name <name>.
                Notify(event::Payload(event::Id::BeginTest, n));
                vector<uint64_t> run_times(t_d.m_num_runs);
                for (auto& rt : run_times) {
                        rt = t_d.m_test_factory().Run();
                }

                // Runs for this test done.
                Notify(event::Payload(event::Id::EndTest, n, run_times));
        }

        // End output.
        Notify(event::Payload(event::Id::EndBench));
}

} // namespace myhayai
