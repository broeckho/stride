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
 * Implementation for the SurveySeeder class.
 */

#include "SurveySeeder.h"

#include "pop/Population.h"
#include "util/RNManager.h"

#include <trng/uniform_int_dist.hpp>

using namespace boost::property_tree;
using namespace stride::util;
using namespace std;

namespace stride {

void SurveySeeder::Seed(const boost::property_tree::ptree& config_pt, shared_ptr<Population> pop, RNManager& rn_manager,
                        std::shared_ptr<spdlog::logger> contact_logger)
{
        const auto   max_pop_index = static_cast<unsigned int>(pop->size() - 1);
        auto         int_generator = rn_manager.GetGenerator(trng::uniform_int_dist(0, max_pop_index));
        const string log_level     = config_pt.get<string>("run.log_level", "None");
        Population&  population    = *pop;

        if (log_level == "Contacts" || log_level == "SusceptibleContacts") {
                const auto num_participants = config_pt.get<unsigned int>("run.num_participants_survey");

                // Use while-loop to get 'num_participant' unique participants (default sampling is with replacement).
                // A for loop will not do because we might draw the same person twice.
                auto num_samples = 0U;
                while (num_samples < num_participants) {
                        Person& p = population[int_generator()];
                        if (!p.IsParticipatingInSurvey()) {
                                p.ParticipateInSurvey();
                                contact_logger->info("[PART] {}", p.GetId());
                                contact_logger->info("[PART] {} {} {} {} {}", p.GetId(), p.GetAge(), p.GetGender(),
                                                     p.GetPoolId(ContactPoolType::Id::School),
                                                     p.GetPoolId(ContactPoolType::Id::Work));
                                num_samples++;
                        }
                }
        }
}

} // namespace stride
