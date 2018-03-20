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

#include "pop/Population.h"
#include "sim/Simulator.h"
#include "sim/SimulatorBuilder.h"
#include "util/RunConfigPtree.h"

#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>
#include <tuple>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace ::testing;
using boost::property_tree::ptree;

namespace Tests {

class BatchRuns : public ::testing::TestWithParam<tuple<const char*, unsigned int>>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

        /// Scenario config and test target number and margin of tolerance in percent (of target value).
        static tuple<ptree, unsigned int, double> ScenarioData(const string& tag)
        {
                //ptree        pt     = tag.substr(0, 2) != "r0" ? BasicConfig1() : BasicConfig2();

                ptree        pt     = tag.substr(0, 2) != "r0"
                                      ? RunConfigPtree::CreateTestsBasic1() : RunConfigPtree::CreateTestsBasic2();
                unsigned int target = 0U;
                double       margin = 0.1;

                // Influenza data
                if (tag == "influenza_a") {
                        target = 1085U;
                }
                if (tag == "influenza_b") {
                        pt.put("run.seeding_rate", 0.0);
                        target = 0U;
                }
                if (tag == "influenza_c") {
                        pt.put("run.seeding_rate", (1 - 0.9991) / 100);
                        pt.put("run.immunity_rate", 0.9991);
                        target = 5U;
                }

                // Measles data.
                if (tag == "measles_16") {
                        pt.put("run.disease_config_file", "disease_measles.xml");
                        pt.put("run.r0", 16U);
                        target = 599900U;
                }
                if (tag == "measles_60") {
                        pt.put("run.disease_config_file", "disease_measles.xml");
                        pt.put("run.r0", 60U);
                        target = 600000U;
                }

                // Run_r0 data
                if (tag == "r0_0") {
                        pt.put("run.r0", 0.0);
                        target = 1200U;
                }
                if (tag == "r0_4") {
                        pt.put("run.r0", 4.0);
                        target = 21000U;
                }
                if (tag == "r0_8") {
                        pt.put("run.r0", 8.0);
                        target = 90354U;
                }
                if (tag == "r0_12") {
                        pt.put("run.r0", 12.0);
                        target = 110745U;
                }
                if (tag == "r0_16") {
                        pt.put("run.r0", 16.0);
                        target = 117153U;
                }

                return make_tuple(pt, target, margin);
        };

protected:
        /// Destructor has to be virtual.
        ~BatchRuns() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST_P(BatchRuns, Run)
{
        tuple<const char*, unsigned int> t(GetParam());

        // -----------------------------------------------------------------------------------------
        // Prepare test configuration.
        // -----------------------------------------------------------------------------------------
        const string test_tag    = get<0>(t);
        const auto   num_threads = get<1>(t);
        omp_set_num_threads(num_threads);
        omp_set_schedule(omp_sched_static, 1);

        // -----------------------------------------------------------------------------------------
        // Scenario configuration and target numbers.
        // -----------------------------------------------------------------------------------------
        const auto d         = ScenarioData(test_tag);
        auto       config_pt = get<0>(d);
        const auto target    = get<1>(d);
        const auto margin    = get<2>(d);

        // -----------------------------------------------------------------------------------------
        // Initialize the simulator.
        // -----------------------------------------------------------------------------------------
        cout << " ----> test_tag: " << test_tag << endl << " ----> threadcount:  " << num_threads << endl;
        config_pt.put("run.num_threads", num_threads);
        SimulatorBuilder builder(config_pt, nullptr);
        const auto       sim = builder.Build();

        // -----------------------------------------------------------------------------------------
        // Run the simulation and release loggers.
        // -----------------------------------------------------------------------------------------
        const auto num_days = config_pt.get<unsigned int>("run.num_days");
        for (unsigned int i = 0; i < num_days; i++) {
                sim->TimeStep();
        }
        spdlog::drop_all();

        // -----------------------------------------------------------------------------------------
        // Check resuts against target number.
        // -----------------------------------------------------------------------------------------
        const unsigned int result = sim->GetPopulation()->GetInfectedCount();
        EXPECT_NEAR(result, target, target * margin) << "!! CHANGES:" << test_tag << " with threads: " << num_threads;
}

namespace {

const char* tags_influenza[] = {"influenza_a", "influenza_b", "influenza_c"};

const char* tags_measles[] = {"measles_16", "measles_60"};

const char* tags_r0[] = {"r0_0", "r0_4", "r0_8", "r0_12", "r0_16"};

#ifdef _OPENMP
unsigned int threads[]{1U, 4U};
#else
unsigned int threads[]{1U};
#endif
} // namespace

INSTANTIATE_TEST_CASE_P(Run_influenza, BatchRuns, Combine(ValuesIn(tags_influenza), ValuesIn(threads)));

INSTANTIATE_TEST_CASE_P(Run_measles, BatchRuns, Combine(ValuesIn(tags_measles), ValuesIn(threads)));

INSTANTIATE_TEST_CASE_P(Run_r0, BatchRuns, Combine(ValuesIn(tags_r0), ValuesIn(threads)));

} // namespace Tests
