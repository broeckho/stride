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

#include "calendar/DaysOffStandard.h"
#include "contact/ContactType.h"
#include "contact/InfectorExec.h"
#include "pop/Population.h"
#include "sim/SimBuilder.h"
#include "util/RunConfigManager.h"

#include <omp.h>
#include <utility>

namespace stride {

using namespace std;
using namespace util;
using namespace ContactLogMode;

Sim::Sim()
    : m_config(), m_contact_log_mode(Id::None), m_num_threads(1U), m_track_index_case(false),
      m_adaptive_symptomatic_behavior(false), m_calendar(nullptr), m_contact_profiles(), m_handlers(), m_infector(),
      m_population(nullptr), m_rn_man(), m_transmission_profile(), m_public_health_agency()
{
}

std::shared_ptr<Sim> Sim::Create(const boost::property_tree::ptree& config, shared_ptr<Population> pop,
                                 util::RnMan rnMan)
{
        struct make_shared_enabler : public Sim
        {
                explicit make_shared_enabler() : Sim() {}
        };
        shared_ptr<Sim> sim = make_shared<make_shared_enabler>();
        SimBuilder(config).Build(sim, std::move(pop), std::move(rnMan));
        return sim;
}

void Sim::TimeStep()
{
        // Logic where you compute (on the basis of input/config for initial day or on the basis of
        // number of sick persons, duration of epidemic etc) what kind of DaysOff scheme you apply.
        const auto daysOff     = std::make_shared<DaysOffStandard>(m_calendar);
        const bool isWorkOff   = daysOff->IsWorkOff();
        const bool isSchoolOff = daysOff->IsSchoolOff();

        // To be used in update of population & contact pools.
        Population& population    = *m_population;
        auto&       poolSys       = population.RefPoolSys();
        auto        contactLogger = population.RefContactLogger();
        const auto  simDay        = m_calendar->GetSimulationDay();
        const auto& infector      = *m_infector;

#pragma omp parallel num_threads(m_num_threads)
        {
                // Update health status and presence/absence in contact pools
                // depending on health status, work/school day and whether
                // we want to track index cases without adaptive behavior
#pragma omp for schedule(static)
                for (size_t i = 0; i < population.size(); ++i) {
                        population[i].Update(isWorkOff, isSchoolOff, m_adaptive_symptomatic_behavior);
                }

                // after the health update, let the public health agency perform their work
                m_public_health_agency.Exec(m_population, m_rn_man, simDay);

                // Infector updates individuals for contacts & transmission within each pool.
                // Skip pools with id = 0, because it means Not Applicable.
                const auto thread_num = static_cast<unsigned int>(omp_get_thread_num());
                for (auto typ : ContactType::IdList) {
                        if ((typ == ContactType::Id::Workplace && isWorkOff) ||
                            (typ == ContactType::Id::K12School && isSchoolOff) ||
                            (typ == ContactType::Id::College && isSchoolOff)) {
                                continue;
                        }
#pragma omp for schedule(static)
                        for (size_t i = 1; i < poolSys.RefPools(typ).size(); i++) { // NOLINT
                                infector(poolSys.RefPools(typ)[i], m_contact_profiles[typ], m_transmission_profile,
                                         m_handlers[thread_num], simDay, contactLogger);
                        }
                }
        }

        m_population->RefContactLogger()->flush();
        m_calendar->AdvanceDay();
}

} // namespace stride
