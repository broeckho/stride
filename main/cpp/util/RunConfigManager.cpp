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

#include "RunConfigManager.h"

#include "util/ConfigInfo.h"
#include "util/FileSys.h"

#include <boost/property_tree/xml_parser.hpp>
#include <initializer_list>
#include <iostream>
#include <sha1.h>
#include <sstream>

using namespace boost::property_tree;
using namespace std;

namespace stride {
namespace util {

void RunConfigManager::CleanConfigFile(ptree pt)
{
        pt.sort();
        const string sha1  = RunConfigManager::ToShortSha1(pt);
        const string fName = sha1 + ".xml";
        cerr << "Rewriting config to file " << fName << " in current directory." << endl;
        FileSys::WritePtreeFile(fName, pt);
}

ptree RunConfigManager::Create(const std::string& configName)
{
        ptree pt;
        if (configName == "TestsInfluenza") {
                pt = CreateTestsInfluenza();
        } else if (configName == "TestsMeasles") {
                pt = CreateTestsMeasles();
        } else if (configName == "BenchMeasles") {
                cout << "bench measles" << endl;
                pt = CreateBenchMeasles();
                cout << "bench measles" << endl;
        } else {
                throw runtime_error("RunConfigManager::Create> Unknown string.");
        }
        return pt;
}

ptree RunConfigManager::CreateBenchInfluenza()
{
        ptree pt;

        pt.put("stride.pop.rng_seed", 20);
        pt.put("stride.pop.rng_type", "lcg64");

        pt.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
        pt.put("run.behaviour_policy", "NoBehaviour");
        pt.put("run.belief_policy.name", "NoBelief");
        pt.put("run.contact_log_level", "None");
        pt.put("run.contact_output_file", "false");
        pt.put("run.disease_config_file", "disease_influenza.xml");
        pt.put("run.global_information_policy", "NoGlobalInformation");
        pt.put("run.holidays_file", "holidays_none.json");
        pt.put("run.immunity_link_probability", 0);
        pt.put("run.immunity_profile", "Random");
        pt.put("run.immunity_rate", 0.0);
        pt.put("run.local_information_policy", "NoLocalInformation");
        pt.put("run.num_days", 30U);
        pt.put("run.num_participants_survey", 10);
        pt.put("run.num_threads", 1);
        pt.put("run.output_prefix", "BatchRuns");
        pt.put("run.population_file", "pop_flanders600.csv");
        pt.put("run.rng_seed", 2015);
        pt.put("run.rng_type", "mrg2");
        pt.put("run.r0", 3.0);
        pt.put("run.seeding_rate", 0.0009);
        pt.put("run.seeding_age_min", 1);
        pt.put("run.seeding_age_max", 99);
        pt.put("run.start_date", "2017-01-01");
        pt.put("run.stride_log_level", "info");
        pt.put("run.track_index_case", false);
        pt.put("run.use_install_dirs", true);
        pt.put("run.vaccine_profile", "None");

        return pt;
}

ptree RunConfigManager::CreateBenchMeasles()
{
        ptree pt;

        pt.put("stride.pop.rng_seed", 20);
        pt.put("stride.pop.rng_type", "lcg64");

        pt.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
        pt.put("run.behaviour_policy", "NoBehaviour");
        pt.put("run.belief_policy.name", "NoBelief");
        // pt.put("run.contact_log_level", "All");
        pt.put("run.contact_log_level", "None");
        pt.put("run.contact_output_file", "false");
        pt.put("run.disease_config_file", "disease_measles.xml");
        pt.put("run.global_information_policy", "NoGlobalInformation");
        pt.put("run.holidays_file", "holidays_none.json");
        pt.put("run.immunity_profile", "None");
        pt.put("run.immunity_rate", 0.01);
        pt.put("run.local_information_policy", "NoLocalInformation");
        pt.put("run.num_days", 30U);
        pt.put("run.num_participants_survey", 10);
        pt.put("run.num_threads", 1U);
        pt.put("run.output_prefix", "bench");
        pt.put("run.population_file", "pop_flanders1600.csv");
        pt.put("run.rng_seed", 1U);
        pt.put("run.rng_type", "mrg2");
        pt.put("run.r0", 15U);
        pt.put("run.seeding_age_max", 99);
        pt.put("run.seeding_age_min", 1);
        pt.put("run.seeding_rate", 0.02);
        pt.put("run.start_date", "2017-01-01");
        pt.put("run.stride_log_level", "info");
        pt.put("run.track_index_case", false);
        pt.put("run.use_install_dirs", true);
        pt.put("run.vaccine_link_probability", 0);
        pt.put("run.vaccine_profile", "Random");
        pt.put("run.vaccine_rate", 0.0);

        return pt;
}

vector<unsigned int> RunConfigManager::CreateNumThreads(unsigned int max)
{
        max = std::max(max, ConfigInfo::NumberAvailableThreads());
        initializer_list<unsigned int> num{1U};

        if (max >= 2) {
                num = {1U, 2U};
        }
        if (max >= 4) {
                num = {1U, 2U, 3U, 4U};
        }
        if (max >= 8) {
                num = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U};
        }
        if (max >= 12) {
                num = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 10U, 12U};
        }
        if (max >= 16) {
                num = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 10U, 12U, 16U};
        }

        return num;
}

ptree RunConfigManager::CreateTestsInfluenza()
{
        ptree pt;

        pt.put("stride.pop.rng_seed", 10);
        pt.put("stride.pop.rng_type", "lcg64");

        pt.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
        pt.put("run.behaviour_policy", "NoBehaviour");
        pt.put("run.belief_policy.name", "NoBelief");
        pt.put("run.contact_log_level", "None");
        pt.put("run.contact_output_file", "false");
        pt.put("run.disease_config_file", "disease_influenza.xml");
        pt.put("run.global_information_policy", "NoGlobalInformation");
        pt.put("run.holidays_file", "holidays_none.json");
        pt.put("run.immunity_link_probability", 0);
        pt.put("run.immunity_profile", "Random");
        pt.put("run.immunity_rate", 0.0);
        pt.put("run.local_information_policy", "NoLocalInformation");
        pt.put("run.num_days", 30U);
        pt.put("run.num_participants_survey", 10);
        pt.put("run.num_threads", 1);
        pt.put("run.output_prefix", "BenchRuns");
        pt.put("run.population_file", "pop_flanders600.csv");
        pt.put("run.rng_seed", 2001);
        pt.put("run.rng_type", "mrg2");
        pt.put("run.r0", 3.0);
        pt.put("run.seeding_rate", 0.0009);
        pt.put("run.seeding_age_min", 1);
        pt.put("run.seeding_age_max", 99);
        pt.put("run.start_date", "2017-01-01");
        pt.put("run.stride_log_level", "info");
        pt.put("run.track_index_case", false);
        pt.put("run.use_install_dirs", true);
        pt.put("run.vaccine_profile", "None");

        return pt;
}

ptree RunConfigManager::CreateTestsMeasles()
{
        ptree pt;

        pt.put("stride.pop.rng_seed", 20);
        pt.put("stride.pop.rng_type", "lcg64");

        pt.put("run.age_contact_matrix_file", "contact_matrix_flanders_subpop.xml");
        pt.put("run.behaviour_policy", "NoBehaviour");
        pt.put("run.belief_policy.name", "NoBelief");
        pt.put("run.contact_log_level", "Transmissions");
        pt.put("run.contact_output_file", "false");
        pt.put("run.disease_config_file", "disease_measles.xml");
        pt.put("run.global_information_policy", "NoGlobalInformation");
        pt.put("run.holidays_file", "holidays_none.json");
        pt.put("run.immunity_rate", 0.8);
        pt.put("run.immunity_profile", "None");
        pt.put("run.local_information_policy", "NoLocalInformation");
        pt.put("run.num_days", 50U);
        pt.put("run.num_participants_survey", 10);
        pt.put("run.num_threads", 1);
        pt.put("run.output_prefix", "BenchRuns");
        pt.put("run.population_file", "pop_flanders600.csv");
        pt.put("run.rng_seed", 1);
        pt.put("run.rng_type", "mrg2");
        pt.put("run.seeding_age_max", 99);
        pt.put("run.seeding_age_min", 1);
        pt.put("run.seeding_rate", 0.002);
        pt.put("run.start_date", "2017-01-01");
        pt.put("run.stride_log_level", "info");
        pt.put("run.track_index_case", false);
        pt.put("run.use_install_dirs", true);
        pt.put("run.vaccine_link_probability", 0);
        pt.put("run.vaccine_profile", "Random");
        pt.put("run.vaccine_rate", 0.8);

        return pt;
}

ptree RunConfigManager::FromString(const string& s)
{
        ptree         pt;
        istringstream is(s);
        read_xml(is, pt);
        return pt;
}

string RunConfigManager::ToString(const ptree& pt)
{
        ostringstream ss;
        write_xml(ss, pt, xml_writer_make_settings<ptree::key_type>(' ', 8));
        return ss.str();
}

std::string RunConfigManager::ToSha1(const boost::property_tree::ptree& pt) { return sha1(ToString(pt)); }

std::string RunConfigManager::ToShortSha1(const boost::property_tree::ptree& pt, unsigned int n)
{
        return ToSha1(pt).substr(0, n);
}

} // namespace util
} // namespace stride
