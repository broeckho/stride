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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */
/**
 * @file
 * Implementation of ptree utils.
 */

#include "RunConfigPtree.h"

using namespace boost::property_tree;
using namespace std;

namespace stride {
namespace util {

const ptree& RunConfigPtree::CreateTestsBasic1()
{
        static ptree pt_config;
        if (pt_config.empty()) {

                pt_config.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
                pt_config.put("run.behaviour_policy", "NoBehaviour");
                pt_config.put("run.belief_policy.name", "NoBelief");
                pt_config.put("run.contact_log_level", "None");
                pt_config.put("run.contact_output_file", "false");
                pt_config.put("run.disease_config_file", "disease_influenza.xml");
                pt_config.put("run.global_information_policy", "NoGlobalInformation");
                pt_config.put("run.holidays_file", "holidays_none.json");
                pt_config.put("run.immunity_link_probability", 0);
                pt_config.put("run.immunity_profile", "Random");
                pt_config.put("run.immunity_rate", 0.0);
                pt_config.put("run.local_information_policy", "NoLocalInformation");
                pt_config.put("run.num_days", 30U);
                pt_config.put("run.num_participants_survey", 10);
                pt_config.put("run.num_threads", 1U);
                pt_config.put("run.output_prefix", "BatchRuns");
                pt_config.put("run.population_file", "pop_flanders600.csv");
                pt_config.put("run.rng_seed", 2015U);
                pt_config.put("run.r0", 3.0);
                pt_config.put("run.seeding_rate", 0.0009);
                pt_config.put("run.seeding_age_min", 1);
                pt_config.put("run.seeding_age_max", 99);
                pt_config.put("run.start_date", "2017-01-01");
                pt_config.put("run.stride_log_level", "error");
                pt_config.put("run.track_index_case", false);
                pt_config.put("run.use_install_dirs", true);
                pt_config.put("run.vaccine_profile", "None");
        }
        return pt_config;
}

const ptree& RunConfigPtree::CreateTestsBasic2()
{
        static ptree pt_config;
        if (pt_config.empty()) {
                pt_config.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
                pt_config.put("run.behaviour_policy", "NoBehaviour");
                pt_config.put("run.belief_policy.name", "NoBelief");
                pt_config.put("run.contact_log_level", "Transmissions");
                pt_config.put("run.contact_output_file", "false");
                pt_config.put("run.disease_config_file", "disease_measles.xml");
                pt_config.put("run.global_information_policy", "NoGlobalInformation");
                pt_config.put("run.holidays_file", "holidays_none.json");
                pt_config.put("run.immunity_rate", 0.8);
                pt_config.put("run.immunity_profile", "None");
                pt_config.put("run.local_information_policy", "NoLocalInformation");
                pt_config.put("run.num_days", 50U);
                pt_config.put("run.num_participants_survey", 10);
                pt_config.put("run.num_threads", 1U);
                pt_config.put("run.output_prefix", "BatchRuns");
                pt_config.put("run.population_file", "pop_flanders600.csv");
                pt_config.put("run.rng_seed", 1U);
                pt_config.put("run.seeding_age_max", 99);
                pt_config.put("run.seeding_age_min", 1);
                pt_config.put("run.seeding_rate", 0.002);
                pt_config.put("run.start_date", "2017-01-01");
                pt_config.put("run.stride_log_level", "error");
                pt_config.put("run.track_index_case", false);
                pt_config.put("run.use_install_dirs", true);
                pt_config.put("run.vaccine_link_probability", 0);
                pt_config.put("run.vaccine_profile", "Random");
                pt_config.put("run.vaccine_rate", 0.8);
        }
        return pt_config;
}

} // namespace util
} // namespace stride
