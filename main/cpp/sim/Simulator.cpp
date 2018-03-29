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
#include "core/ContactHandler.h"
#include "core/Infector.h"
#include "pop/Population.h"
#include <trng/uniform01_dist.hpp>
#include <omp.h>

namespace stride {

using namespace std;
using namespace trng;
using namespace util;

/// Default constructor for empty Simulator.
Simulator::Simulator()
    : m_config_pt(), m_contact_log_mode(ContactLogMode::Id::Null), m_contact_logger(nullptr), m_contact_profiles(),
      m_disease_profile(), m_num_threads(1U), m_track_index_case(false), m_calendar(), m_operational(), m_rn_manager(),
      m_sim_day(0U), m_population(nullptr), m_households(), m_school_pools(), m_work_pools(), m_primary_community(),
      m_secondary_community(), m_local_information_policy()
{
}

/// Run one time step, computing full simulation (default) or only index case.
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
        if (m_local_information_policy == "NoLocalInformation") {
                if (m_track_index_case) {
                        switch (m_contact_log_mode) {
                        case Id::SusceptibleContacts:
                                UpdateContactPools<Id::SusceptibleContacts, NoLocalInformation, true>();
                                break;
                        case Id::Contacts: UpdateContactPools<Id::Contacts, NoLocalInformation, true>(); break;
                        case Id::Transmissions:
                                UpdateContactPools<Id::Transmissions, NoLocalInformation, true>();
                                break;
                        case Id::None: UpdateContactPools<Id::None, NoLocalInformation, true>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                } else {
                        switch (m_contact_log_mode) {
                        case Id::SusceptibleContacts:
                                UpdateContactPools<Id::SusceptibleContacts, NoLocalInformation, false>();
                                break;
                        case Id::Contacts: UpdateContactPools<Id::Contacts, NoLocalInformation, false>(); break;
                        case Id::Transmissions:
                                UpdateContactPools<Id::Transmissions, NoLocalInformation, false>();
                                break;
                        case Id::None: UpdateContactPools<Id::None, NoLocalInformation, false>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                }
        } else if (m_local_information_policy == "LocalDiscussion") {
                if (m_track_index_case) {
                        switch (m_contact_log_mode) {
                        case Id::SusceptibleContacts:
                                UpdateContactPools<Id::SusceptibleContacts, LocalDiscussion, true>();
                                break;
                        case Id::Contacts: UpdateContactPools<Id::Contacts, LocalDiscussion, true>(); break;
                        case Id::Transmissions: UpdateContactPools<Id::Transmissions, LocalDiscussion, true>(); break;
                        case Id::None: UpdateContactPools<Id::None, LocalDiscussion, true>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                } else {
                        switch (m_contact_log_mode) {
                        case Id::SusceptibleContacts:
                                UpdateContactPools<Id::SusceptibleContacts, LocalDiscussion, false>();
                                break;
                        case Id::Contacts: UpdateContactPools<Id::Contacts, LocalDiscussion, false>(); break;
                        case Id::Transmissions: UpdateContactPools<Id::Transmissions, LocalDiscussion, false>(); break;
                        case Id::None: UpdateContactPools<Id::None, LocalDiscussion, false>(); break;
                        default: throw std::runtime_error(std::string(__func__) + "Log mode screwed up!");
                        }
                }
        } else {
                throw std::runtime_error(std::string(__func__) + "No valid local information policy!");
        }

        Notify(static_cast<unsigned int>(m_calendar->GetSimulationDay()));

        m_calendar->AdvanceDay();
}

/// Update the contacts in the given contactpools.
template <ContactLogMode::Id log_level, typename local_information_policy, bool track_index_case>
void Simulator::UpdateContactPools()
{
        // Contact handlers, each boud to a generator bound to a different random engine stream.
        vector<ContactHandler> handlers;
        for (size_t i = 0; i < m_num_threads; i++) {
                // RN generators with double in [0.0, 1.0) each bound to a different stream.
                auto gen = m_rn_manager.GetGenerator(trng::uniform01_dist<double>(), i);
                handlers.emplace_back(ContactHandler(gen));
        }

#pragma omp parallel num_threads(m_num_threads)
        {
                const auto thread = static_cast<unsigned int>(omp_get_thread_num());

#pragma omp for schedule(runtime)
                for (size_t i = 0; i < m_households.size(); i++) { // NOLINT
                        Infector<log_level, track_index_case, local_information_policy>::Exec(
                            m_households[i], m_disease_profile, handlers[thread], m_calendar, m_contact_logger);
                }

#pragma omp for schedule(runtime)
                for (size_t i = 0; i < m_school_pools.size(); i++) { // NOLINT
                        Infector<log_level, track_index_case, local_information_policy>::Exec(
                            m_school_pools[i], m_disease_profile, handlers[thread], m_calendar, m_contact_logger);
                }

#pragma omp for schedule(runtime)
                for (size_t i = 0; i < m_work_pools.size(); i++) { // NOLINT
                        Infector<log_level, track_index_case, local_information_policy>::Exec(
                            m_work_pools[i], m_disease_profile, handlers[thread], m_calendar, m_contact_logger);
                }

#pragma omp for schedule(runtime)
                for (size_t i = 0; i < m_primary_community.size(); i++) { // NOLINT
                        Infector<log_level, track_index_case, local_information_policy>::Exec(
                            m_primary_community[i], m_disease_profile, handlers[thread], m_calendar, m_contact_logger);
                }

#pragma omp for schedule(runtime)
                for (size_t i = 0; i < m_secondary_community.size(); i++) { // NOLINT
                        Infector<log_level, track_index_case, local_information_policy>::Exec(
                            m_secondary_community[i], m_disease_profile, handlers[thread], m_calendar,
                            m_contact_logger);
                }
        }
}

} // namespace stride
