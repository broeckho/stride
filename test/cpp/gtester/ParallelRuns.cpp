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
#include "util/ConfigInfo.h"
#include "util/RunConfigManager.h"

#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include <omp.h>
#include <tuple>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace ::testing;
using boost::property_tree::ptree;

namespace Tests {

class ParallelRuns : public ::testing::TestWithParam<const char*>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

protected:
        /// Destructor has to be virtual.
        ~ParallelRuns() override = default;

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST_P(ParallelRuns, Run)
{
        // -----------------------------------------------------------------------------------------
        // Prepare test configuration.
        // -----------------------------------------------------------------------------------------
        const auto           maxNumThreads = ConfigInfo::NumberAvailableThreads();
        vector<unsigned int> numThreads;
        if (maxNumThreads >= 4) {
                numThreads.push_back(maxNumThreads / 2);
                numThreads.push_back(maxNumThreads);
        } else {
                numThreads.push_back(maxNumThreads);
        }

        // -----------------------------------------------------------------------------------------
        // Scenario configuration and target numbers.
        // -----------------------------------------------------------------------------------------
        const string testTag  = GetParam();
        const auto   d        = ScenarioData::Get(testTag);
        auto         configPt = get<0>(d);
        const auto   target   = get<1>(d);
        const auto   margin   = get<2>(d);

        // -----------------------------------------------------------------------------------------
        // Run simulator and check result.
        // -----------------------------------------------------------------------------------------
        for (const auto n : numThreads) {
                configPt.put("run.num_threads", n);
                auto runner = make_shared<SimRunner>(configPt, Population::Create(configPt));
                runner->Run();
                const auto result = runner->GetSim()->GetPopulation()->GetInfectedCount();
                EXPECT_NEAR(result, target, target * margin)
                    << "!! CHANGES:" << testTag << " with #threads: " << n << endl;
        }
}

namespace {

const char* tags_influenza[] = {"influenza_a", "influenza_c"};

const char* tags_measles[] = {"measles_16"};

const char* tags_r0[] = {"r0_0", "r0_4", "r0_8", "r0_12", "r0_16"};

} // namespace

INSTANTIATE_TEST_CASE_P(influenza, ParallelRuns, ValuesIn(tags_influenza));

INSTANTIATE_TEST_CASE_P(measles, ParallelRuns, ValuesIn(tags_measles));

INSTANTIATE_TEST_CASE_P(r0, ParallelRuns, ValuesIn(tags_r0));

} // namespace Tests
