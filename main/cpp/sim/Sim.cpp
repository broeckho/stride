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
 *  Copyright 2017, 2018 Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation for the Simulator class.
 */

#include "Sim.h"

#include "behaviour/information_policies/LocalDiscussion.h"
#include "behaviour/information_policies/NoLocalInformation.h"
#include "calendar/DaysOffStandard.h"
#include "pool/ContactPoolType.h"
#include "pop/Population.h"
#include "sim/SimBuilder.h"
#include "util/RunConfigManager.h"

#include <omp.h>

namespace stride {

using namespace std;
using namespace trng;
using namespace util;
using namespace ContactLogMode;

Sim::Sim()
    : m_config_pt(), m_contact_log_mode(Id::None), m_num_threads(1U), m_track_index_case(false), m_local_info_policy(),
      m_calendar(nullptr), m_contact_profiles(), m_population(nullptr), m_rn_manager(), m_transmission_profile()
{
}

std::shared_ptr<Sim> Sim::Create(const boost::property_tree::ptree& configPt, shared_ptr<Population> pop)
{
        struct make_shared_enabler : public Sim
        {
        };
        shared_ptr<Sim> sim = make_shared<make_shared_enabler>();
        SimBuilder(configPt).Build(sim, pop);
        return sim;
}

std::shared_ptr<Sim> Sim::Create(const string& configString, shared_ptr<Population> pop)
{
        return Create(RunConfigManager::FromString(configString), pop);
}

void Sim::TimeStep()
{
        std::shared_ptr<DaysOffInterface> daysOff{nullptr};

        // Logic where you compute (on the basis of input/config for initial day or on the basis of
        // number of sick persons, duration of epidemic etc) what kind of DaysOff scheme you apply.
        daysOff                = std::make_shared<DaysOffStandard>(m_calendar);
        const bool isWorkOff   = daysOff->IsWorkOff();
        const bool isSchoolOff = daysOff->IsSchoolOff();

        // To be used in update of population & contact pools.
        Population& population    = *m_population;
        auto&       poolSys       = population.GetContactPoolSys();
        auto        contactLogger = population.GetContactLogger();
        const auto  simDay        = m_calendar->GetSimulationDay();
        const auto& infector      = *m_infector;

#pragma omp parallel num_threads(m_num_threads)
        {
                // Update health status presence/absence in pools
                // depending on health status, work/school day.
#pragma omp for schedule(static)
                for (size_t i = 0; i < population.size(); ++i) {
                        population[i].Update(isWorkOff, isSchoolOff);
                }

                // Loop over types of contact pool (household, etc.) and over pools of that type.
                // Infector updates individuals for contacts & transmission within each pool.
                // Skip pools with id = 0, because it means Not Applicable.
                const auto thread_num = static_cast<unsigned int>(omp_get_thread_num());
                for (auto typ : ContactPoolType::IdList) {
                        if ((typ==ContactPoolType::Id::Work && isWorkOff) ||
                                (typ==ContactPoolType::Id::School && isSchoolOff)) {
                                continue;
                        }
#pragma omp for schedule(static)
                        for (size_t i = 1; i < poolSys[typ].size(); i++) { // NOLINT
                                infector(poolSys[typ][i], m_contact_profiles[typ], m_transmission_profile,
                                         m_handlers[thread_num], simDay, contactLogger);
                        }
                }
        }

        m_population->GetContactLogger()->flush();
        m_calendar->AdvanceDay();
}

} // namespace stride
