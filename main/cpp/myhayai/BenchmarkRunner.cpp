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
 * Implementation file for Benchmarker.
 */

#include "BenchmarkRunner.h"

#include "Id.hpp"
#include "InfoFactory.h"
#include "Test.h"
#include "TestDescriptors.h"
#include "TestFactory.h"
#include "TestResult.h"

using namespace std;
using namespace std::chrono;

namespace myhayai {

const TestDescriptors& BenchmarkRunner::GetTestDescriptors() const { return m_test_descriptors; }

BenchmarkRunner& BenchmarkRunner::Instance()
{
        static BenchmarkRunner singleton;
        return singleton;
}

bool BenchmarkRunner::RegisterTest(const string& groupName, const string& testName, size_t numRuns,
                                   TestFactory testFactory, InfoFactory infoFactory, bool isDisabled)
{
        TestDescriptor d(groupName, testName, numRuns, std::move(testFactory), std::move(infoFactory), isDisabled);
        const auto     ret = Instance().m_test_descriptors.emplace(make_pair(d.GetCanonicalName(), d));
        return ret.second;
}

void BenchmarkRunner::RunTests(const vector<string>& canonicalNames)
{
        // Setup.
        auto test_descriptors = Instance().GetTestDescriptors();
        Notify(event::Payload(event::Id::BeginBench));

        // Run through all the test_descriptors in ascending order.
        for (const auto& name : canonicalNames) {
                // If test is disabled, then skip it.
                const auto& t_d = test_descriptors[name];
                if (t_d.m_is_disabled) {
                        Notify(event::Payload(event::Id::SkipTest, name));
                        continue;
                }

                // Runs for test with canonical name <name>.
                Notify(event::Payload(event::Id::BeginTest, name));
                TestResult runTimes(t_d.m_num_runs);
                try {
                        for (auto& rt : runTimes) {
                                rt = t_d.m_test_factory().Run<steady_clock>();
                        }
                } catch (exception& e) {
                        Notify(event::Payload(event::Id::AbortTest, name, e.what()));
                        continue;
                }

                // Runs for this test done.
                Notify(event::Payload(event::Id::EndTest, name, runTimes));
        }

        // End output.
        Notify(event::Payload(event::Id::EndBench));
}

} // namespace myhayai
