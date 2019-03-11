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
#include "util/Exception.h"
#include "util/RnMan.h"

#include <boost/property_tree/ptree.hpp>
#include <trng/uniform_int_dist.hpp>
#include <cassert>

using namespace boost::property_tree;
using namespace stride::util;
using namespace stride::ContactType;
using namespace std;

namespace stride {

SurveySeeder::SurveySeeder(const ptree& config, RnMan& rnMan) : m_config(config), m_rn_man(rnMan) {}

shared_ptr<Population> SurveySeeder::Seed(shared_ptr<Population> pop)
{
        const string logLevel = m_config.get<string>("run.contact_log_level", "None");
        if (logLevel != "None") {
                Population& population  = *pop;
                auto&       poolSys     = population.GetContactPoolSys();
                auto&       logger      = population.GetContactLogger();
                const auto  popCount    = static_cast<unsigned int>(population.size() - 1);
                const auto  numSurveyed = m_config.get<unsigned int>("run.num_participants_survey");

                assert((popCount >= 1U) && "SurveySeeder> Population count zero unacceptable.");
                assert((popCount >= numSurveyed) && "SurveySeeder> Pop count has to exceeed number of surveyed.");

                // Use while-loop to get 'participants' unique participants (default sampling is with replacement).
                // A for loop will not do because we might draw the same person twice.
                auto numSamples = 0U;
                auto generator  = m_rn_man[0].variate_generator(trng::uniform_int_dist(0, static_cast<int>(popCount)));

                while (numSamples < numSurveyed) {
                        Person& p = population[generator()];
                        if (p.IsSurveyParticipant()) {
                                continue;
                        }
                        p.ParticipateInSurvey();

                        const auto h = p.GetHealth();
                        logger->info("[PART] {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}", p.GetId(),
                                     p.GetAge(), p.GetPoolId(Id::Household), p.GetPoolId(Id::K12School),
                                     p.GetPoolId(Id::College), p.GetPoolId(Id::Workplace), h.IsSusceptible(),
                                     h.IsInfected(), h.IsInfectious(), h.IsRecovered(), h.IsImmune(),
                                     h.GetStartInfectiousness(), h.GetStartSymptomatic(), h.GetEndInfectiousness(),
                                     h.GetEndSymptomatic(),
                                     poolSys[Id::Household][p.GetPoolId(Id::Household)].GetSize(),
                                     poolSys[Id::K12School][p.GetPoolId(Id::K12School)].GetSize(),
                                     poolSys[Id::College][p.GetPoolId(Id::College)].GetSize(),
                                     poolSys[Id::Workplace][p.GetPoolId(Id::Workplace)].GetSize(),
                                     poolSys[Id::PrimaryCommunity][p.GetPoolId(Id::PrimaryCommunity)].GetSize(),
                                     poolSys[Id::SecondaryCommunity][p.GetPoolId(Id::SecondaryCommunity)].GetSize());

                        numSamples++;
                }
        }
        return pop;
}

} // namespace stride
