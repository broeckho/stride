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
#include "util/InstallDirs.h"
#include "util/PtreeUtils.h"
#include "util/StringUtils.h"

#include <iostream>
#include <spdlog/spdlog.h>

namespace stride {

using namespace std;
using namespace util;
using namespace boost::property_tree;

/**
 * Initializes a Population: add persons, set immunity, seed infection.
 *
 * @param pt_config     Property_tree with general configuration settings.
 * @param pt_disease    Property_tree with disease configuration settings.
 * @return              Pointer to the initialized population.
 */
std::shared_ptr<Population> PopulationBuilder::Build(const ptree& pt_config, const ptree& pt_disease, util::Random& rng)
{
        // ------------------------------------------------
        // Setup.
        // ------------------------------------------------
        std::cout << "Setup" << std::endl;
        const auto pop = make_shared<Population>();
        Population& population = *pop;

        const auto seeding_rate = pt_config.get<double>("run.seeding_rate");
        const string disease_config_file = pt_config.get<string>("run.disease_config_file");

        // ------------------------------------------------
        // Logger.
        // ------------------------------------------------
        std::cout << "Logger" << std::endl;
        const shared_ptr<spdlog::logger> logger = spdlog::get("contact_logger");

        //------------------------------------------------
        // Check input.
        //------------------------------------------------
        std::cout << "Check input" << std::endl;
        bool status = (seeding_rate <= 1);
        if (!status) {
                throw runtime_error(string(__func__) + "> Bad input data.");
        }

        //------------------------------------------------
        // Add persons to population.
        //------------------------------------------------
        std::cout << "Population file" << std::endl;
        const auto file_name = pt_config.get<string>("run.population_file");

        const auto file_path = InstallDirs().GetDataDir() /= file_name;
        if (!is_regular_file(file_path)) {
                throw runtime_error(string(__func__) + "> Population file " + file_path.string() + " not present.");
        }

        boost::filesystem::ifstream pop_file;
        pop_file.open(file_path.string());
        if (!pop_file.is_open()) {
                throw runtime_error(string(__func__) + "> Error opening population file " + file_path.string());
        }

        const auto distrib_start_infectiousness =
            PtreeUtils::GetDistribution(pt_disease, "disease.start_infectiousness");
        const auto distrib_start_symptomatic = PtreeUtils::GetDistribution(pt_disease, "disease.start_symptomatic");
        const auto distrib_time_infectious = PtreeUtils::GetDistribution(pt_disease, "disease.time_infectious");
        const auto distrib_time_symptomatic = PtreeUtils::GetDistribution(pt_disease, "disease.time_symptomatic");

        const auto pt_belief = pt_config.get_child("run.belief_policy");

        string line;
        getline(pop_file, line); // step over file header
        unsigned int person_id = 0U;

        while (getline(pop_file, line)) {
                // Make use of stochastic disease characteristics.
                const auto start_infectiousness = Sample(rng, distrib_start_infectiousness);
                const auto start_symptomatic = Sample(rng, distrib_start_symptomatic);
                const auto time_infectious = Sample(rng, distrib_time_infectious);
                const auto time_symptomatic = Sample(rng, distrib_time_symptomatic);

                const auto values = Split(line, ",");
                const auto risk_averseness = (values.size() <= 6) ? 0.0 : FromString<double>(values[6]);
                auto age = FromString<unsigned int>(values[0]);
                auto household_id = FromString<unsigned int>(values[1]);
                auto school_id = FromString<unsigned int>(values[2]);
                auto work_id = FromString<unsigned int>(values[3]);
                auto primary_community_id = FromString<unsigned int>(values[4]);
                auto secondary_community_id = FromString<unsigned int>(values[5]);

                population.CreatePerson(person_id, age, household_id, school_id, work_id, primary_community_id,
                                        secondary_community_id, start_infectiousness, start_symptomatic,
                                        time_infectious, time_symptomatic,
                                        risk_averseness, pt_belief);

                ++person_id;
        }

        pop_file.close();

        //------------------------------------------------
        // Customize the population.
        //------------------------------------------------
        std::cout << "Customize" << std::endl;
        const auto max_population_index = static_cast<unsigned int>(population.size() - 1);
        if (max_population_index <= 1U) {
                throw runtime_error(string(__func__) + "> Problem with population size.");
        }

        //------------------------------------------------
        // Set participants in social contact survey.
        //------------------------------------------------
        std::cout << "Survey" << std::endl;
        const string log_level = pt_config.get<string>("run.log_level", "None");
        if (log_level == "Contacts") {
                const unsigned int num_participants{
                    static_cast<unsigned int>(pt_config.get<double>("run.num_participants_survey"))};

                // use a while-loop to obtain 'num_participant' unique participants (default
                // sampling is with
                // replacement)
                // A for loop will not do because we might draw the same person twice.
                unsigned int num_samples{0};
                while (num_samples < num_participants) {
                        Person& p{population[rng(max_population_index)]};
                        if (!p.IsParticipatingInSurvey()) {
                                p.ParticipateInSurvey();
                                logger->info("[PART] {}", p.GetId());
                                logger->info("[PART] {} {} {} {} {}", p.GetId(), p.GetAge(), p.GetGender(),
                                             p.GetClusterId(ClusterType::Id::School),
                                             p.GetClusterId(ClusterType::Id::Work));
                                num_samples++;
                        }
                }
        }

        //------------------------------------------------
        // Done
        //------------------------------------------------
        return pop;
}

/// Sample from the distribution
unsigned int PopulationBuilder::Sample(util::Random& rng, const std::vector<double>& distribution)
{
        double random_value = rng.NextDouble();
        for (unsigned int i = 0; i < distribution.size(); i++) {
                if (random_value <= distribution[i]) {
                        return i;
                }
        }
        cerr << "WARNING: PROBLEM WITH DISEASE DISTRIBUTION [PopulationBuilder]" << endl;
        return static_cast<unsigned int>(distribution.size());
}

} // namespace stride
