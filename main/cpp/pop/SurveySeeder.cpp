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
using namespace stride::ContactPoolType;
using namespace std;

namespace stride {

void SurveySeeder::Seed(const boost::property_tree::ptree& configPt, shared_ptr<Population> pop, RNManager& rnManager)
{
        const string log_level = configPt.get<string>("run.contact_log_level", "None");
        if (log_level == "All" || log_level == "Susceptibles") {

                Population& population   = *pop;
                auto&       logger       = population.GetContactLogger();
                const auto  max_index    = static_cast<unsigned int>(population.size() - 1);
                auto        generator    = rnManager.GetGenerator(trng::uniform_int_dist(0, max_index));
                const auto  participants = configPt.get<unsigned int>("run.num_participants_survey");

                // Use while-loop to get 'participants' unique participants (default sampling is with replacement).
                // A for loop will not do because we might draw the same person twice.
                auto num_samples = 0U;
                while (num_samples < participants) {
                        Person& p = population[generator()];
                        if (!p.IsSurveyParticipant()) {
                                p.ParticipateInSurvey();
                                logger->info("[PART] {} {} {} {} {}", p.GetId(), p.GetAge(), p.GetGender(),
                                             p.GetPoolId(Id::School), p.GetPoolId(Id::Work));
                                num_samples++;
                        }
                }
        }
}

} // namespace stride
