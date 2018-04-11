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

#include "myhayai/Id.hpp"
#include "myhayai/InfoFactory.hpp"
#include "myhayai/Test.hpp"
#include "myhayai/TestDescriptors.hpp"
#include "myhayai/TestFactory.hpp"
#include "myhayai/TestResult.hpp"

using namespace std;
using namespace std::chrono;

namespace myhayai {

const TestDescriptors& Benchmarker::GetTestDescriptors() const { return m_test_descriptors; }

Benchmarker& Benchmarker::Instance()
{
        static Benchmarker singleton;
        return singleton;
}

bool Benchmarker::RegisterTest(const char* groupName, const char* testName, size_t numRuns, TestFactory testFactory,
                               InfoFactory infoFactory, bool isDisabled)
{
        TestDescriptor d(groupName, testName, numRuns, std::move(testFactory), std::move(infoFactory), isDisabled);
        const auto     ret = Instance().m_test_descriptors.emplace(make_pair(d.GetCanonicalName(), d));
        return ret.second;
}

void Benchmarker::RunTests(const vector<string>& canonicalNames)
{
        // Setup.
        auto test_descriptors = Instance().GetTestDescriptors();
        Notify(event::Payload(event::Id::BeginBench));

        // Run through all the test_descriptors in ascending order.
        for (const auto& n : canonicalNames) {

                const auto& t_d = test_descriptors[n];

                // If test is disabled, then skip it.
                if (t_d.m_is_disabled) {
                        Notify(event::Payload(event::Id::SkipTest, n));
                        continue;
                }

                // Number of runs for test with canonical name <name>.
                Notify(event::Payload(event::Id::BeginTest, n));
                TestResult runTimes(t_d.m_num_runs);
                for (auto& rt : runTimes) {
                        rt = t_d.m_test_factory().Run<steady_clock>();
                }

                // Runs for this test done.
                Notify(event::Payload(event::Id::EndTest, n, runTimes));
        }

        // End output.
        Notify(event::Payload(event::Id::EndBench));
}

} // namespace myhayai
