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

#include "sim/SimulatorBuilder.h"

#include <gtest/gtest.h>
#include <omp.h>
#include <spdlog/spdlog.h>

using namespace std;
using namespace stride;
using namespace ::testing;

namespace Tests {

class BatchDemos : public ::testing::TestWithParam<tuple<string, unsigned int>>
{
public:
        /// TestCase set up.
        static void SetUpTestCase() {}

        /// Tearing down TestCase
        static void TearDownTestCase() {}

        /// Reference results for the various scenarios.
        const map<string, unsigned int>& ReferenceResults()
        {
                static const map<string, unsigned int> results{{"default", 2000U},
                                                               {"seeding_rate", 0U},
                                                               {"immunity_rate", 5U},
                                                               {"measles", 346100U},
                                                               {"maximum", 600000}};
                return results;
        };

protected:
        /// Destructor has to be virtual.
        ~BatchDemos() override {}

        /// Set up for the test fixture
        void SetUp() override {}

        /// Tearing down the test fixture
        void TearDown() override  {}
};

TEST_P(BatchDemos, Run)
{
        // -----------------------------------------------------------------------------------------
        // Prepare test configuration.
        // -----------------------------------------------------------------------------------------
        tuple<string, unsigned int> t(GetParam());
        const string                test_tag    = get<0>(t);
        const unsigned int          num_threads = get<1>(t);
        omp_set_num_threads(num_threads);
        omp_set_schedule(omp_sched_static, 1);

        // -----------------------------------------------------------------------------------------
        // Setup configuration.
        // -----------------------------------------------------------------------------------------
        boost::property_tree::ptree pt_config;
        pt_config.put("run.rng_seed", 2015U);
        pt_config.put("run.r0", 3.0);
        pt_config.put("run.seeding_rate", 0.0009);
        pt_config.put("run.seeding_age_min", 1);
        pt_config.put("run.seeding_age_max", 99);
        pt_config.put("run.immunity_profile", "Random");
        pt_config.put("run.immunity_rate", 0.0);
        pt_config.put("run.immunity_link_probability", 0);
        pt_config.put("run.vaccine_profile", "None");
        pt_config.put("run.population_file", "pop_flanders600.csv");
        pt_config.put("run.num_days", 30U);
        pt_config.put("run.output_prefix", "test");
        pt_config.put("run.disease_config_file", "disease_influenza.xml");
        pt_config.put("run.num_participants_survey", 10);
        pt_config.put("run.start_date", "2017-01-01");
        pt_config.put("run.holidays_file", "holidays_none.json");
        pt_config.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
        pt_config.put("run.log_level", "None");
        pt_config.put("run.local_information_policy", "NoLocalInformation");
        pt_config.put("run.global_information_policy", "NoGlobalInformation");
        pt_config.put("run.belief_policy.name", "NoBelief");
        pt_config.put("run.behaviour_policy", "NoBehaviour");
        bool track_index_case = false;

        // -----------------------------------------------------------------------------------------
        // Override scenario settings.
        // -----------------------------------------------------------------------------------------
        if (test_tag == "default") {
                // do nothing
        }
        if (test_tag == "seeding_rate") {
                pt_config.put("run.seeding_rate", 0.0);
        }
        if (test_tag == "immunity_rate") {
                pt_config.put("run.seeding_rate", (1 - 0.9991) / 100);
                pt_config.put("run.immunity_rate", 0.9991);
        }
        if (test_tag == "measles") {
                pt_config.put("run.disease_config_file", "disease_measles.xml");
                pt_config.put("run.r0", 16U);
        }
        if (test_tag == "maximum") {
                pt_config.put("run.disease_config_file", "disease_measles.xml");
                pt_config.put("run.r0", 60U);
        }

        // -----------------------------------------------------------------------------------------
        // Initialize the logger.
        // -----------------------------------------------------------------------------------------
        spdlog::set_async_mode(1048576);
        auto file_logger = spdlog::rotating_logger_mt("contact_logger", "test_logfile", numeric_limits<size_t>::max(),
                                                      numeric_limits<size_t>::max());
        file_logger->set_pattern("%v"); // Remove meta data from log => time-stamp of logging

        // -----------------------------------------------------------------------------------------
        // Initialize the simulation.
        // -----------------------------------------------------------------------------------------
        cout << "Building the simulator. " << endl;
        auto sim = SimulatorBuilder::Build(pt_config, num_threads, track_index_case);
        cout << "Done building the simulator. " << endl << endl;

        // -----------------------------------------------------------------------------------------
        // Run the simulation.
        // -----------------------------------------------------------------------------------------
        const unsigned int num_days = pt_config.get<unsigned int>("run.num_days");
        for (unsigned int i = 0; i < num_days; i++) {
                sim->TimeStep();
        }

        // -----------------------------------------------------------------------------------------
        // Release and close logger.
        // -----------------------------------------------------------------------------------------
        spdlog::drop_all();

        // -----------------------------------------------------------------------------------------
        // Check resuts against reference reseults.
        // -----------------------------------------------------------------------------------------
        const unsigned int res = sim->GetPopulation()->GetInfectedCount();
        EXPECT_NEAR(res, ReferenceResults().at(test_tag), ReferenceResults().at(test_tag) * 0.1)
            << "!! CHANGES for " << test_tag << "with threads: " << num_threads;
}

namespace {

#ifdef _OPENMP
unsigned int threads[]{1U, 4U, 8U};
#else
unsigned int threads[]{1U};
#endif
} // namespace

INSTANTIATE_TEST_CASE_P(Run_default, BatchDemos,
                        ::testing::Combine(::testing::Values(string("default")), ::testing::ValuesIn(threads)));

INSTANTIATE_TEST_CASE_P(Run_seeding_rate, BatchDemos,
                        ::testing::Combine(::testing::Values(string("seeding_rate")), ::testing::ValuesIn(threads)));

INSTANTIATE_TEST_CASE_P(Run_immunity_rate, BatchDemos,
                        ::testing::Combine(::testing::Values(string("immunity_rate")), ::testing::ValuesIn(threads)));

INSTANTIATE_TEST_CASE_P(Run_measles, BatchDemos,
                        ::testing::Combine(::testing::Values(string("measles")), ::testing::ValuesIn(threads)));

INSTANTIATE_TEST_CASE_P(Run_maximum, BatchDemos,
                        ::testing::Combine(::testing::Values(string("maximum")), ::testing::ValuesIn(threads)));

} // namespace Tests
