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
 *  Copyright 2017, 2018 Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Implementation of scenario tests running in batch mode.
 */

#include "ScenarioData.h"
#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"
#include "util/RunConfigManager.h"

#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include <cmath>
#include <iomanip>
#include <tuple>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace ::testing;
using boost::property_tree::ptree;

namespace Tests {

class ScenarioRuns : public ::testing::TestWithParam<const char*>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~ScenarioRuns() override = default;

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

class RunsDefault : public ScenarioRuns
{
};

class RunsGeoPop : public ScenarioRuns
{
};

void RunTest(const string& testTag, tuple<ptree, unsigned int, double> d, unsigned int numThreads)
{
        // -----------------------------------------------------------------------------------------
        // Scenario configuration and target numbers.
        // -----------------------------------------------------------------------------------------
        auto       configPt = get<0>(d);
        const auto target   = get<1>(d);
        const auto margin   = get<2>(d);
        configPt.put("run.num_threads", numThreads);

        // -----------------------------------------------------------------------------------------
        // Actual simulator run.
        // -----------------------------------------------------------------------------------------
        stride::util::RnMan rn_manager;
        rn_manager.Initialize(RnMan::Info{configPt.get<string>("run.rng_seed", "1,2,3,4"),
                                          configPt.get<string>("run.rng_state", ""),
                                          configPt.get<unsigned int>("run.num_threads")});
        auto runner = make_shared<SimRunner>(configPt, Population::Create(configPt, rn_manager), rn_manager);
        runner->Run();

        // -----------------------------------------------------------------------------------------
        // Check results against target number (|res - target| < target * margin).
        // -----------------------------------------------------------------------------------------
        const unsigned int res = runner->GetSim()->GetPopulation()->GetInfectedCount();
        EXPECT_NEAR(res, target, target * margin);
        cerr.setf(ios_base::scientific, ios_base::floatfield);
        cerr.precision(2);
        cerr << "Test: " << testTag << ", number of threads: " << numThreads
                << ",  result: " << res
                << ",  target: " << target
                << ",  % delta: " << fabs(static_cast<double>(res)-static_cast<double>(target))/(1.0e-8+fabs(target))
                << ",  margin: " << margin<< endl;
}

TEST_P(RunsDefault, SingleThreadRun)
{
        const string testTag = GetParam();
        RunTest(testTag, ScenarioData::Get(testTag), 1U);
}

TEST_P(RunsGeoPop, SingleThreadRun)
{
        const string testTag = string(GetParam()).append("_geopop");
        RunTest(testTag,  ScenarioData::Get(testTag), 1U);
}

#ifdef _OPENMP
TEST_P(RunsDefault, MultiThreadRuns)
{
        const auto numThreads = RunConfigManager::CreateNumThreads();
        const string testTag = GetParam();
        for (const auto n : numThreads) {
                if (n == 1U) { // Skip single thread test
                        continue;
                }
                RunTest(testTag, ScenarioData::Get(testTag), n);
        }
}

TEST_P(RunsGeoPop, MultiThreadRuns)
{
        const auto numThreads = RunConfigManager::CreateNumThreads();
        const string testTag = string(GetParam()).append("_geopop");
        for (const auto n : numThreads) {
                if (n == 1U) { // Skip single thread test
                        continue;
                }
                RunTest(testTag, ScenarioData::Get(testTag), n);
        }
}
#endif

namespace {

const char* tags_influenza[] = {"influenza_a", "influenza_b", "influenza_c"};

const char* tags_measles[] = {"measles_16", "measles_26"};

const char* tags_r0[] = {"r0_0", "r0_4", "r0_8", "r0_12", "r0_16"};

} // namespace

INSTANTIATE_TEST_CASE_P(influenza, RunsDefault, ValuesIn(tags_influenza));

INSTANTIATE_TEST_CASE_P(measles, RunsDefault, ValuesIn(tags_measles));

INSTANTIATE_TEST_CASE_P(r0, RunsDefault, ValuesIn(tags_r0));

INSTANTIATE_TEST_CASE_P(influenza_geopop, RunsGeoPop, ValuesIn(tags_influenza));

INSTANTIATE_TEST_CASE_P(measles_geopop, RunsGeoPop, ValuesIn(tags_measles));

INSTANTIATE_TEST_CASE_P(r0_geopop, RunsGeoPop, ValuesIn(tags_r0));

} // namespace Tests
