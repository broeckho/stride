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
 * Implementation of tests to check the relation R0-attack rate.
 */

#include "sim/SimulatorBuilder.h"

#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>
#include <tuple>

namespace Tests {

using namespace std;
using namespace stride;
using namespace ::testing;

using boost::property_tree::ptree;

class InfluenceR0Runs : public ::testing::TestWithParam<tuple<const char*, unsigned int>>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

        /// Basic config, R0 will change over the various scenarios.
        static ptree BasicConfig()
        {
                static ptree pt_config;
                if (pt_config.empty()) {
                        pt_config.put("run.rng_seed", 1U);
                        pt_config.put("run.seeding_rate", 0.002);
                        pt_config.put("run.seeding_age_min", 1);
                        pt_config.put("run.seeding_age_max", 99);
                        pt_config.put("run.immunity_rate", 0.8);
                        pt_config.put("run.immunity_profile", "None");
                        pt_config.put("run.vaccine_rate", 0.8);
                        pt_config.put("run.vaccine_profile", "Random");
                        pt_config.put("run.vaccine_link_probability", 0);
                        pt_config.put("run.population_file", "pop_flanders600.csv");
                        pt_config.put("run.num_days", 50U);
                        pt_config.put("run.output_prefix", "");
                        pt_config.put("run.disease_config_file", "disease_measles.xml");
                        pt_config.put("run.generate_person_file", 1);
                        pt_config.put("run.num_participants_survey", 10);
                        pt_config.put("run.start_date", "2017-01-01");
                        pt_config.put("run.holidays_file", "holidays_none.json");
                        pt_config.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
                        pt_config.put("run.log_level", "Transmissions");
                        pt_config.put("run.local_information_policy", "NoLocalInformation");
                        pt_config.put("run.global_information_policy", "NoGlobalInformation");
                        pt_config.put("run.belief_policy.name", "NoBelief");
                        pt_config.put("run.behaviour_policy", "NoBehaviour");
                }
                return pt_config;
        }

        /// Scenario config and test target number and margin of tolerance in percent (of target value).
        static tuple<ptree, unsigned int, double> ScenarioData(const string& tag)
        {
                ptree        pt     = BasicConfig();
                unsigned int target = 0U;
                double       margin = 0.1;

                // Targets originally obtained by running python script examples/runExperimentsR0.py
                if (tag == "r0_0") {
                        pt.put("run.r0", 0.0);
                        target = 1200U;
                }
                if (tag == "r0_4") {
                        pt.put("run.r0", 4.0);
                        target = 19387U;
                }
                if (tag == "r0_8") {
                        pt.put("run.r0", 8.0);
                        target = 80878U;
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
        }

protected:
        /// Destructor has to be virtual.
        ~InfluenceR0Runs() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override {}
};

TEST_P(InfluenceR0Runs, Run)
{
        tuple<const char*, unsigned int> t(GetParam());

        // -------------------------------------------------------------------------------------
        // Prepare test configuration.
        // -------------------------------------------------------------------------------------
        const string test_tag{get<0>(t)};
        const auto   num_threads = get<1>(t);
        omp_set_num_threads(num_threads);
        omp_set_schedule(omp_sched_static, 1);

        // -------------------------------------------------------------------------------------
        // Scenario test configuration and target number.
        // -------------------------------------------------------------------------------------
        const auto d         = ScenarioData(test_tag);
        const auto pt_config = get<0>(d);
        const auto target    = get<1>(d);
        const auto margin    = get<2>(d);

        // -------------------------------------------------------------------------------------
        // Initialize the logger.
        // -------------------------------------------------------------------------------------
        spdlog::set_async_mode(1048576);
        auto file_logger = spdlog::rotating_logger_mt("contact_logger", "test_logfile", numeric_limits<size_t>::max(),
                                                      numeric_limits<size_t>::max());
        file_logger->set_pattern("%v"); // Remove meta data from log => time-stamp of logging

        // -----------------------------------------------------------------------------------------
        // Initialize the simulator.
        // -----------------------------------------------------------------------------------------
        cout << "Building the simulator. " << endl;
        cout << " ----> test_tag: " << test_tag << endl << " ----> threadcount:  " << num_threads << endl;
        auto sim = SimulatorBuilder::Build(pt_config, num_threads);
        cout << "Done building the simulator" << endl;

        // -----------------------------------------------------------------------------------------
        // Run the simulation and release logger.
        // -----------------------------------------------------------------------------------------
        const unsigned int num_days = pt_config.get<unsigned int>("run.num_days");
        for (unsigned int i = 0; i < num_days; i++) {
                sim->TimeStep();
        }
        spdlog::drop_all();

        // -----------------------------------------------------------------------------------------
        // Check resuts against target number.
        // -----------------------------------------------------------------------------------------
        const unsigned int res = sim->GetPopulation()->GetInfectedCount();
        EXPECT_NEAR(res, target, target * margin) << "!! CHANGES for " << test_tag << "with threads: " << num_threads;
}

namespace {
const char* tags[] = {"r0_0", "r0_4", "r0_8", "r0_12", "r0_16"};

#ifdef _OPENMP
unsigned int threads[]{1U, 4U};
#else
unsigned int threads[]{1U};
#endif

} // namespace

INSTANTIATE_TEST_CASE_P(Run, InfluenceR0Runs, Combine(ValuesIn(tags), ValuesIn(threads)));

} // namespace Tests
