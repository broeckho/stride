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
#include "util/RnMan.h"

#include <boost/property_tree/ptree.hpp>
#include <trng/uniform_int_dist.hpp>

using namespace boost::property_tree;
using namespace stride::util;
using namespace stride::ContactType;
using namespace std;

namespace stride {

SurveySeeder::SurveySeeder(const ptree& configPt, RnMan& rnManager) : m_config_pt(configPt), m_rn_manager(rnManager) {}

shared_ptr<Population> SurveySeeder::Seed(shared_ptr<Population> pop)
{
        const string log_level = m_config_pt.get<string>("run.contact_log_level", "None");
        if (log_level != "None") {
                Population& population   = *pop;
                auto&       poolSys      = population.GetContactPoolSys();
                auto&       logger       = population.GetContactLogger();
                const auto  max_index    = static_cast<unsigned int>(population.size() - 1);
                auto        generator    = m_rn_manager[0].variate_generator(trng::uniform_int_dist(0, max_index));
                const auto  participants = m_config_pt.get<unsigned int>("run.num_participants_survey");

                // Use while-loop to get 'participants' unique participants (default sampling is with replacement).
                // A for loop will not do because we might draw the same person twice.
                auto num_samples = 0U;
                while (num_samples < participants) {
                        Person& p = population[generator()];
                        if (!p.IsSurveyParticipant()) {
                                p.ParticipateInSurvey();

                                // TODO: create a more elegant solution
                                // - gengeopop population ==>> unique pool id over all pool types, so ID != index in
                                // poolType-vector
                                // - default population   ==>> unique pool id per pool type, so ID == index in
                                // poolType-vector
                                if (p.GetPoolId(Id::SecondaryCommunity) < poolSys[Id::SecondaryCommunity].size()) {
                                        logger->info(
                                            "[PART] {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}",
                                            p.GetId(), p.GetAge(), p.GetGender(), p.GetPoolId(Id::Household),
                                            p.GetPoolId(Id::K12School), p.GetPoolId(Id::College),
                                            p.GetPoolId(Id::Workplace), p.GetHealth().IsSusceptible(),
                                            p.GetHealth().IsInfected(), p.GetHealth().IsInfectious(),
                                            p.GetHealth().IsRecovered(), p.GetHealth().IsImmune(),
                                            p.GetHealth().GetStartInfectiousness(), p.GetHealth().GetStartSymptomatic(),
                                            p.GetHealth().GetEndInfectiousness(), p.GetHealth().GetEndSymptomatic(),
                                            poolSys[Id::Household][p.GetPoolId(Id::Household)].GetSize(),
                                            poolSys[Id::K12School][p.GetPoolId(Id::K12School)].GetSize(),
                                            poolSys[Id::College][p.GetPoolId(Id::College)].GetSize(),
                                            poolSys[Id::Workplace][p.GetPoolId(Id::Workplace)].GetSize(),
                                            poolSys[Id::PrimaryCommunity][p.GetPoolId(Id::PrimaryCommunity)].GetSize(),
                                            poolSys[Id::SecondaryCommunity][p.GetPoolId(Id::SecondaryCommunity)]
                                                .GetSize());
                                } else {
                                        logger->info(
                                            "[PART] {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}",
                                            p.GetId(), p.GetAge(), p.GetGender(), p.GetPoolId(Id::Household),
                                            p.GetPoolId(Id::K12School), p.GetPoolId(Id::College),
                                            p.GetPoolId(Id::Workplace), p.GetHealth().IsSusceptible(),
                                            p.GetHealth().IsInfected(), p.GetHealth().IsInfectious(),
                                            p.GetHealth().IsRecovered(), p.GetHealth().IsImmune(),
                                            p.GetHealth().GetStartInfectiousness(), p.GetHealth().GetStartSymptomatic(),
                                            p.GetHealth().GetEndInfectiousness(), p.GetHealth().GetEndSymptomatic(), -1,
                                            -1, -1, -1, -1, -1, -1);
                                }

                                num_samples++;
                        }
                }
        }
        return pop;
}

} // namespace stride
