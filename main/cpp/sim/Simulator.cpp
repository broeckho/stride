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
 * Implementation for the Simulator class.
 */

#include "Simulator.h"

#include "behaviour/information_policies/LocalDiscussion.h"
#include "behaviour/information_policies/NoLocalInformation.h"
#include "calendar/Calendar.h"
#include "calendar/DaysOffStandard.h"
#include "contact/ContactHandler.h"
#include "contact/Infector.h"
#include "pool/ContactPoolType.h"
#include "pop/Population.h"

#include <trng/uniform01_dist.hpp>
#include <omp.h>

namespace stride {

using namespace std;
using namespace trng;
using namespace util;

Simulator::Simulator()
    : m_config_pt(), m_contact_log_mode(ContactLogMode::Id::None), m_contact_logger(nullptr), m_contact_profiles(),
      m_num_threads(1U), m_track_index_case(false), m_transmission_profile(), m_calendar(), m_rn_manager(),
      m_sim_day(0U), m_population(nullptr), m_pool_sys(), m_local_info_policy()
{
}

void Simulator::TimeStep()
{
        std::shared_ptr<DaysOffInterface> days_off{nullptr};

        // Logic where you compute (on the basis of input/config for initial day or on the basis of
        // number of sick persons, duration of epidemic etc) what kind of DaysOff scheme you apply.
        // If we want to make this independent of contacpools, then the days_off object has to be
        // passed into the Update function.
        days_off                 = std::make_shared<DaysOffStandard>(m_calendar);
        const bool is_work_off   = days_off->IsWorkOff();
        const bool is_school_off = days_off->IsSchoolOff();

        // Update individual's health status & presence in contactpools.
        for (auto& p : *m_population) {
                p.Update(is_work_off, is_school_off);
        }

        using Id = ContactLogMode::Id;
        if (m_local_info_policy == "NoLocalInformation") {
                if (m_track_index_case) {
                        switch (m_contact_log_mode) {
                        case Id::Susceptibles: UpdatePools<Id::Susceptibles, NoLocalInformation, true>(); break;
                        case Id::All: UpdatePools<Id::All, NoLocalInformation, true>(); break;
                        case Id::Transmissions: UpdatePools<Id::Transmissions, NoLocalInformation, true>(); break;
                        case Id::None: UpdatePools<Id::None, NoLocalInformation, true>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                } else {
                        switch (m_contact_log_mode) {
                        case Id::Susceptibles: UpdatePools<Id::Susceptibles, NoLocalInformation, false>(); break;
                        case Id::All: UpdatePools<Id::All, NoLocalInformation, false>(); break;
                        case Id::Transmissions: UpdatePools<Id::Transmissions, NoLocalInformation, false>(); break;
                        case Id::None: UpdatePools<Id::None, NoLocalInformation, false>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                }
        } else if (m_local_info_policy == "LocalDiscussion") {
                if (m_track_index_case) {
                        switch (m_contact_log_mode) {
                        case Id::Susceptibles: UpdatePools<Id::Susceptibles, LocalDiscussion, true>(); break;
                        case Id::All: UpdatePools<Id::All, LocalDiscussion, true>(); break;
                        case Id::Transmissions: UpdatePools<Id::Transmissions, LocalDiscussion, true>(); break;
                        case Id::None: UpdatePools<Id::None, LocalDiscussion, true>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                } else {
                        switch (m_contact_log_mode) {
                        case Id::Susceptibles: UpdatePools<Id::Susceptibles, LocalDiscussion, false>(); break;
                        case Id::All: UpdatePools<Id::All, LocalDiscussion, false>(); break;
                        case Id::Transmissions: UpdatePools<Id::Transmissions, LocalDiscussion, false>(); break;
                        case Id::None: UpdatePools<Id::None, LocalDiscussion, false>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                }
        } else {
                throw std::runtime_error(std::string(__func__) + "No valid local information policy!");
        }

        Notify(static_cast<unsigned int>(m_calendar->GetSimulationDay()));

        m_calendar->AdvanceDay();
}

template <ContactLogMode::Id log_level, typename local_information_policy, bool track_index_case>
void Simulator::UpdatePools()
{
        using namespace stride::ContactPoolType;

        // Contact handlers, each boud to a generator bound to a different random engine stream.
        vector<ContactHandler> handlers;
        for (size_t i = 0; i < m_num_threads; i++) {
                // RN generators with double in [0.0, 1.0) each bound to a different stream.
                auto gen = m_rn_manager.GetGenerator(trng::uniform01_dist<double>(), i);
                handlers.emplace_back(ContactHandler(gen));
        }

        // Loop over the various types of contact pool systems (household, school, work, etc
        // The inner loop over the pools in each system is parallellized providing OpenMP is available.
        // Infector updates individuals for contacts & transmission within a pool.

        const auto sim_day = m_calendar->GetSimulationDay();

#pragma omp parallel num_threads(m_num_threads)
        {
                const auto thread = static_cast<unsigned int>(omp_get_thread_num());
                for (auto typ : ContactPoolType::IdList) {
#pragma omp for schedule(runtime)
                        for (size_t i = 0; i < m_pool_sys[typ].size(); i++) { // NOLINT
                                Infector<log_level, track_index_case, local_information_policy>::Exec(
                                    m_pool_sys[typ][i], m_contact_profiles[typ], m_transmission_profile,
                                    handlers[thread], sim_day, m_contact_logger);
                        }
                }
        }
}

} // namespace stride
