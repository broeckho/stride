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
 *  Copyright 2018, Willem L, Kuylen E, Broeckhove J
 */

/**
 * @file
 * Implementation for the PublicHealthAgency class.
 */

#include "PublicHealthAgency.h"

#include "pop/Population.h"
#include "util/RnMan.h"

#include <trng/uniform01_dist.hpp>
#include <numeric>
#include <vector>

namespace stride {

using namespace std;
using namespace util;

/// Execute
void PublicHealthAgency::Exec(std::shared_ptr<Population> pop, util::RnMan& rnManager, unsigned short int simDay)
{
        PerformCaseFinding(pop, rnManager, simDay);
}

/// Initialize
void PublicHealthAgency::Initialize(const double case_detection_probability)
{
        m_case_detection_probability = case_detection_probability;
}

/// Public Health Strategy: vaccinate the household of a symptomatic case if symptoms started today
void PublicHealthAgency::PerformCaseFinding(std::shared_ptr<Population> pop, util::RnMan& rnMan,
                                            unsigned short int simDay)
{

        // perform case finding, if the probability is > 0.0
        if (m_case_detection_probability > 0.0) {

                using namespace ContactType;
                auto& population         = *pop;
                auto& poolSys            = population.GetContactPoolSys();
                auto  uniform01Generator = rnMan[0].variate_generator(trng::uniform01_dist<double>());
                auto& logger             = pop->GetContactLogger();

                /// To allow iteration over pool types for the PublicHealthAgency.
                std::initializer_list<Id> AgencyPoolIdList{Id::Household};

                for (auto& p_case : population) {
                        if (p_case.GetHealth().IsSymptomatic() && p_case.GetHealth().SymptomsStartedToday()) {

                                for (Id typ : AgencyPoolIdList) {
                                        const auto poolId = p_case.GetPoolId(typ);
                                        if (poolId > 0) {
                                                for (const auto& p_member : poolSys[typ][poolId].GetPool()) {
                                                        if (p_case != *p_member &&
                                                            p_member->GetHealth().IsSusceptible() &&
                                                            uniform01Generator() < m_case_detection_probability) {

                                                                // set immune
                                                                p_member->GetHealth().SetImmune();

                                                                // TODO: check log_level
                                                                logger->info("[VACC] {} {} {} {} {} {} {}",
                                                                             p_member->GetId(), p_member->GetAge(),
                                                                             ToString(typ), poolId, p_case.GetId(),
                                                                             p_case.GetAge(), simDay);
                                                        }
                                                }
                                        }
                                }
                        }
                }
        } // end if-clause whether the case detection probability > 0
}

} /* namespace stride */
