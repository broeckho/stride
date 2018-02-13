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
 *  Copyright 2017, Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Implementation of scenario tests running in batch mode.
 */

#include "sim/SimulatorBuilder.h"

#include <boost/property_tree/ptree.hpp>
#include <gtest/gtest.h>
#include <map>
#include <omp.h>
#include <spdlog/spdlog.h>
#include <string>
#include <tuple>

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

protected:
        /// Destructor has to be virtual.
        virtual ~BatchDemos() {}

        /// Set up for the test fixture
        virtual void SetUp() {}

        /// Tearing down the test fixture
        virtual void TearDown() {}

        // Data members of the test fixture
        static const string       g_population_file;
        static const double       g_r0;
        static const unsigned int g_num_days;
        static const unsigned int g_rng_seed;
        static const double       g_seeding_rate;
        static const double       g_seeding_age_min;
        static const double       g_seeding_age_max;
        static const string       g_immunity_profile;
        static const double       g_immunity_rate;
        static const double       g_immunity_link_probability;
        static const string       g_vaccine_profile;
        static const string       g_disease_config_file;
        static const string       g_output_prefix;
        static const string       g_holidays_file;
        static const unsigned int g_num_participants_survey;
        static const string       g_start_date;
        static const string       g_age_contact_matrix_file;
        static const string       g_log_level;

        static const unsigned int g_rng_seed_adapted;
        static const double       g_seeding_rate_adapted;
        static const double       g_immunity_rate_adapted;
        static const string       g_disease_config_file_adapted;
        static const double       g_transmission_rate_measles;
        static const double       g_transmission_rate_maximum;

        static const string g_local_information_policy;
        static const string g_global_information_policy;
        static const string g_belief_policy;
        static const string g_behaviour_policy;

        static const map<string, unsigned int> g_results;
};

// Default values
const string       BatchDemos::g_population_file           = "pop_flanders600.csv";
const double       BatchDemos::g_r0                        = 2.0;
const unsigned int BatchDemos::g_num_days                  = 30U;
const unsigned int BatchDemos::g_rng_seed                  = 2015U;
const double       BatchDemos::g_seeding_rate              = 0.0009;
const double       BatchDemos::g_seeding_age_min           = 1;
const double       BatchDemos::g_seeding_age_max           = 99;
const string       BatchDemos::g_immunity_profile          = "Random";
const double       BatchDemos::g_immunity_rate             = 0.0;
const double       BatchDemos::g_immunity_link_probability = 0;
const string       BatchDemos::g_vaccine_profile           = "None";
const string       BatchDemos::g_disease_config_file       = "disease_influenza.xml";
const string       BatchDemos::g_output_prefix             = "test";
const string       BatchDemos::g_holidays_file             = "holidays_none.json";
const unsigned int BatchDemos::g_num_participants_survey   = 10;
const string       BatchDemos::g_start_date                = "2017-01-01";
const string       BatchDemos::g_age_contact_matrix_file   = "contact_matrix_flanders_subpop.xml";
const string       BatchDemos::g_log_level                 = "None";
const string       BatchDemos::g_local_information_policy  = "NoLocalInformation";
const string       BatchDemos::g_global_information_policy = "NoGlobalInformation";
const string       BatchDemos::g_belief_policy             = "NoBelief";
const string       BatchDemos::g_behaviour_policy          = "NoBehaviour";

// Adapted values
const double BatchDemos::g_seeding_rate_adapted        = 0.0;
const double BatchDemos::g_immunity_rate_adapted       = 0.9991;
const string BatchDemos::g_disease_config_file_adapted = "disease_measles.xml";
const double BatchDemos::g_transmission_rate_measles   = 16U;
const double BatchDemos::g_transmission_rate_maximum   = 60U;

const map<string, unsigned int> BatchDemos::g_results{make_pair("default", 829), make_pair("seeding_rate", 0),
                                                      make_pair("immunity_rate", 5), make_pair("measles", 346093),
                                                      make_pair("maximum", 600000)};

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
        pt_config.put("run.rng_seed", g_rng_seed);
        pt_config.put("run.r0", g_r0);
        pt_config.put("run.seeding_rate", g_seeding_rate);
        pt_config.put("run.seeding_age_min", g_seeding_age_min);
        pt_config.put("run.seeding_age_max", g_seeding_age_max);
        pt_config.put("run.immunity_profile", g_immunity_profile);
        pt_config.put("run.immunity_rate", g_immunity_rate);
        pt_config.put("run.immunity_link_probability", g_immunity_link_probability);
        pt_config.put("run.vaccine_profile", g_vaccine_profile);
        pt_config.put("run.population_file", g_population_file);
        pt_config.put("run.num_days", g_num_days);
        pt_config.put("run.output_prefix", g_output_prefix);
        pt_config.put("run.disease_config_file", g_disease_config_file);
        pt_config.put("run.num_participants_survey", g_num_participants_survey);
        pt_config.put("run.start_date", g_start_date);
        pt_config.put("run.holidays_file", g_holidays_file);
        pt_config.put("run.age_contact_matrix_file", g_age_contact_matrix_file);
        pt_config.put("run.log_level", g_log_level);
        pt_config.put("run.local_information_policy", g_local_information_policy);
        pt_config.put("run.global_information_policy", g_global_information_policy);
        pt_config.put("run.belief_policy.name", g_belief_policy);
        pt_config.put("run.behaviour_policy", g_behaviour_policy);
        bool track_index_case = false;

        // -----------------------------------------------------------------------------------------
        // Override scenario settings.
        // -----------------------------------------------------------------------------------------
        if (test_tag == "default") {
                // do nothing
        }
        if (test_tag == "seeding_rate") {
                pt_config.put("run.seeding_rate", g_seeding_rate_adapted);
        }
        if (test_tag == "immunity_rate") {
                pt_config.put("run.seeding_rate", (1 - g_immunity_rate_adapted) / 100);
                pt_config.put("run.immunity_rate", g_immunity_rate_adapted);
        }
        if (test_tag == "measles") {
                pt_config.put("run.disease_config_file", g_disease_config_file_adapted);
                pt_config.put("run.r0", g_transmission_rate_measles);
        }
        if (test_tag == "maximum") {
                pt_config.put("run.disease_config_file", g_disease_config_file_adapted);
                pt_config.put("run.r0", g_transmission_rate_maximum);
        }

        // -----------------------------------------------------------------------------------------
        // Initialize the logger.
        // -----------------------------------------------------------------------------------------
        spdlog::set_async_mode(1048576);
        auto file_logger =
            spdlog::rotating_logger_mt("contact_logger", g_output_prefix + "_logfile",
                                       std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max());
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
        // Round up.
        // -----------------------------------------------------------------------------------------
        const unsigned int num_cases = sim->GetPopulation()->GetInfectedCount();
        // ASSERT_NEAR(val1, val2, abs_error)
        ASSERT_NEAR(num_cases, g_results.at(test_tag), g_results.at(test_tag) * 0.5) << "!! CHANGED !!";
}

namespace {

string scenarios[]{"default", "seeding_rate", "immunity_rate", "measles", "maximum"};

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
