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
 * Initialize populations: implementation.
 */

#include "PopulationBuilder.h"

#include "disease/Health.h"
#include "pop/Population.h"
#include "util/FileSys.h"
#include "util/StringUtils.h"

#include <trng/uniform_int_dist.hpp>

namespace stride {

using namespace std;
using namespace util;
using namespace boost::property_tree;

std::shared_ptr<Population> PopulationBuilder::Build(const ptree& config_pt, util::RNManager& rn_manager)
{
        // ------------------------------------------------
        // Setup.
        // ------------------------------------------------
        const auto  pop        = make_shared<Population>();
        Population& population = *pop;
        const auto  belief_pt  = config_pt.get_child("run.belief_policy");

        //------------------------------------------------
        // Seeding rate.
        //------------------------------------------------
        const auto seeding_rate = config_pt.get<double>("run.seeding_rate");
        if (seeding_rate > 1.0) {
                throw runtime_error(string(__func__) + "> Bad input data.");
        }

        //------------------------------------------------
        // Build population from file.
        //------------------------------------------------
        const auto file_name        = config_pt.get<string>("run.population_file");
        const auto use_install_dirs = config_pt.get<bool>("run.use_install_dirs");
        const auto file_path        = (use_install_dirs) ? FileSys::GetDataDir() /= file_name : file_name;
        if (!is_regular_file(file_path)) {
                throw runtime_error(string(__func__) + "> Population file " + file_path.string() + " not present.");
        }

        boost::filesystem::ifstream pop_file;
        pop_file.open(file_path.string());
        if (!pop_file.is_open()) {
                throw runtime_error(string(__func__) + "> Error opening population file " + file_path.string());
        }

        string line;
        getline(pop_file, line); // step over file header
        unsigned int person_id = 0U;

        while (getline(pop_file, line)) {
                const auto values                 = Split(line, ",");
                const auto risk_averseness        = (values.size() <= 6) ? 0.0 : FromString<double>(values[6]);
                const auto age                    = FromString<unsigned int>(values[0]);
                const auto household_id           = FromString<unsigned int>(values[1]);
                const auto school_id              = FromString<unsigned int>(values[2]);
                const auto work_id                = FromString<unsigned int>(values[3]);
                const auto primary_community_id   = FromString<unsigned int>(values[4]);
                const auto secondary_community_id = FromString<unsigned int>(values[5]);

                population.CreatePerson(person_id, age, household_id, school_id, work_id, primary_community_id,
                                        secondary_community_id, Health(), belief_pt, risk_averseness);
                ++person_id;
        }

        pop_file.close();

        //------------------------------------------------
        // Done
        //------------------------------------------------
        return pop;
}

} // namespace stride
