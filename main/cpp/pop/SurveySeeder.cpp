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
                auto&       poolSys     = population.CRefPoolSys();
                auto&       logger      = population.RefContactLogger();
                const auto  popCount    = static_cast<unsigned int>(population.size() - 1);
                const auto  numSurveyed = m_config.get<unsigned int>("run.num_participants_survey");

                assert((popCount >= 1U) && "SurveySeeder> Population count zero unacceptable.");
                assert((popCount >= numSurveyed) && "SurveySeeder> Pop count has to exceeed number of surveyed.");

                // Use while-loop to get 'participants' unique participants (default sampling is with replacement).
                // A for loop will not do because we might draw the same person twice.
                auto numSamples = 0U;
                auto generator  = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(popCount), 0U);

                while (numSamples < numSurveyed) {
                        Person& p = population[generator()];
                        if (p.IsSurveyParticipant()) {
                                continue;
                        }
                        p.ParticipateInSurvey();

                        const auto h    = p.GetHealth();
                        const auto pHH  = p.GetPoolId(Id::Household);
                        const auto pK12 = p.GetPoolId(Id::K12School);
                        const auto pC   = p.GetPoolId(Id::College);
                        const auto pW   = p.GetPoolId(Id::Workplace);
                        const auto pPC  = p.GetPoolId(Id::PrimaryCommunity);
                        const auto pSC  = p.GetPoolId(Id::SecondaryCommunity);
                        logger->info("[PART] {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}", p.GetId(),
                                     p.GetAge(), pHH, pK12, pC, pW, h.IsSusceptible(), h.IsInfected(), h.IsInfectious(),
                                     h.IsRecovered(), h.IsImmune(), h.GetStartInfectiousness(), h.GetStartSymptomatic(),
                                     h.GetEndInfectiousness(), h.GetEndSymptomatic(),
                                     poolSys.CRefPools<Id::Household>()[pHH].GetPool().size(),
                                     poolSys.CRefPools<Id::K12School>()[pK12].GetPool().size(),
                                     poolSys.CRefPools<Id::College>()[pC].GetPool().size(),
                                     poolSys.CRefPools<Id::Workplace>()[pW].GetPool().size(),
                                     poolSys.CRefPools<Id::PrimaryCommunity>()[pPC].GetPool().size(),
                                     poolSys.CRefPools<Id::SecondaryCommunity>()[pSC].GetPool().size());

                        numSamples++;
                }
        }
        return pop;
}

} // namespace stride
