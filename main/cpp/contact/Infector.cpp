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
 * Implementation of Infector algorithms.
 */

#include "Infector.h"

#include "calendar/Calendar.h"
#include "contact/LogPolicy.h"
#include "contact/R0Policy.h"
#include "pool/ContactPool.h"

using namespace std;

namespace {

using namespace stride;
using namespace stride::ContactPoolType;

inline double GetContactRate(const AgeContactProfile& profile, const Person* p, size_t pool_size)
{
        const double reference_num_contacts{profile[EffectiveAge(static_cast<unsigned int>(p->GetAge()))]};
        const double potential_num_contacts{static_cast<double>(pool_size - 1)};

        double individual_contact_rate = reference_num_contacts / potential_num_contacts;
        if (individual_contact_rate >= 1) {
                individual_contact_rate = 0.999;
        }
        // Contacts are reciprocal, so one needs to apply only half of the contacts here.
        individual_contact_rate = individual_contact_rate / 2;
        // Contacts are bi-directional: contact probability for 1=>2 and 2=>1 = indiv_cnt_rate*indiv_cnt_rate
        individual_contact_rate += (individual_contact_rate * individual_contact_rate);

        return individual_contact_rate;
}

} // namespace

namespace stride {

//-------------------------------------------------------------------------------------------------
// Definition for primary template covers the situation for ContactLogMode::None &
// ContactLogMode::Transmissions, both with track_index_case false and true.
// And every local information policy except NoLocalInformation
//-------------------------------------------------------------------------------------------------
template <ContactLogMode::Id LL, bool TIC, typename LIP, bool TO>
void Infector<LL, TIC, LIP, TO>::Exec(ContactPool& pool, const AgeContactProfile& profile,
                                      const TransmissionProfile& trans_profile, ContactHandler& c_handler,
                                      unsigned short int sim_day, shared_ptr<spdlog::logger> c_logger)
{
        using LP = LOG_POLICY<LL>;
        using RP = R0_POLICY<TIC>;

        // set up some stuff
        const auto  p_type    = pool.m_pool_type;
        const auto& p_members = pool.m_members;
        const auto  p_size    = p_members.size();
        const auto  t_rate    = trans_profile.GetRate();

        // check all contacts
        for (size_t i_person1 = 0; i_person1 < p_size; i_person1++) {
                // check if member is present today
                const auto p1 = p_members[i_person1];
                if (p1->IsInPool(p_type)) {
                        const double c_rate = GetContactRate(profile, p1, p_size);
                        // loop over possible contacts (contacts can be initiated by each member)
                        for (size_t i_person2 = 0; i_person2 < p_size; i_person2++) {
                                // check if not the same person
                                if (i_person1 != i_person2) {
                                        // check if member is present today
                                        const auto p2 = p_members[i_person2];
                                        if (p2->IsInPool(p_type)) {
                                                // check for contact
                                                if (c_handler.HasContact(c_rate)) {
                                                        // log contact if person 1 is participating in survey
                                                        if (p1->IsSurveyParticipant()) {
                                                                LP::Contact(c_logger, p1, p2, p_type, sim_day);
                                                        }
                                                        // log contact if person 2 is participating in survey
                                                        if (p2->IsSurveyParticipant()) {
                                                                LP::Contact(c_logger, p2, p1, p_type, sim_day);
                                                        }

                                                        // exchange info about health state & beliefs
                                                        LIP::Update(p1, p2);

                                                        // transmission & infection.
                                                        if (c_handler.HasTransmission(t_rate)) {
                                                                auto& h1 = p1->GetHealth();
                                                                auto& h2 = p2->GetHealth();
                                                                if (h1.IsInfectious() && h2.IsSusceptible()) {
                                                                        LP::Transmission(c_logger, p1, p2, p_type,
                                                                                         sim_day);
                                                                        h2.StartInfection();
                                                                        RP::Exec(p2);
                                                                } else if (h2.IsInfectious() && h1.IsSusceptible()) {
                                                                        LP::Transmission(c_logger, p2, p1, p_type,
                                                                                         sim_day);
                                                                        h1.StartInfection();
                                                                        RP::Exec(p1);
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

//-------------------------------------------------------------------------------------------
// Time optimized implementation for NoLocalInformationPolicy and None || Transmission logging.
//-------------------------------------------------------------------------------------------
template <ContactLogMode::Id LL, bool TIC>
void Infector<LL, TIC, NoLocalInformation, true>::Exec(ContactPool& pool, const AgeContactProfile& profile,
                                                       const TransmissionProfile& trans_profile,
                                                       ContactHandler& c_handler, unsigned short int sim_day,
                                                       shared_ptr<spdlog::logger> c_logger)
{
        using LP = LOG_POLICY<LL>;
        using RP = R0_POLICY<TIC>;

        // check for infected members and sort
        bool   infectious_cases;
        size_t num_cases;
        tie(infectious_cases, num_cases) = pool.SortMembers();

        if (infectious_cases) {
                // set up some stuff
                const auto  c_type    = pool.m_pool_type;
                const auto  c_immune  = pool.m_index_immune;
                const auto& c_members = pool.m_members;
                const auto  c_size    = c_members.size();
                const auto  t_rate    = trans_profile.GetRate();

                // match infectious and susceptible members, skip last part (immune members)
                for (size_t i_infected = 0; i_infected < num_cases; i_infected++) {
                        // check if member is present today
                        const auto p1 = c_members[i_infected];
                        if (p1->IsInPool(c_type)) {
                                auto& h1 = p1->GetHealth();
                                if (h1.IsInfectious()) {
                                        const double c_rate_p1 = GetContactRate(profile, p1, c_size);
                                        // loop over possible susceptible contacts
                                        for (size_t i_contact = num_cases; i_contact < c_immune; i_contact++) {
                                                // check if member is present today
                                                const auto p2 = c_members[i_contact];
                                                if (p2->IsInPool(c_type)) {
                                                        const double c_rate_p2 = GetContactRate(profile, p2, c_size);
                                                        if (c_handler.HasContactAndTransmission(c_rate_p1, t_rate) ||
                                                            c_handler.HasContactAndTransmission(c_rate_p2, t_rate)) {
                                                                auto& h2 = p2->GetHealth();
                                                                if (h1.IsInfectious() && h2.IsSusceptible()) {
                                                                        h2.StartInfection();
                                                                        RP::Exec(p2);
                                                                        LP::Transmission(c_logger, p1, p2, c_type,
                                                                                         sim_day);
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

//--------------------------------------------------------------------------
// All explicit instantiations.
//--------------------------------------------------------------------------
template class Infector<ContactLogMode::Id::None, false, NoLocalInformation>;
template class Infector<ContactLogMode::Id::None, false, LocalDiscussion>;
template class Infector<ContactLogMode::Id::None, true, NoLocalInformation>;
template class Infector<ContactLogMode::Id::None, true, LocalDiscussion>;

template class Infector<ContactLogMode::Id::Transmissions, false, NoLocalInformation>;
template class Infector<ContactLogMode::Id::Transmissions, false, LocalDiscussion>;
template class Infector<ContactLogMode::Id::Transmissions, true, NoLocalInformation>;
template class Infector<ContactLogMode::Id::Transmissions, true, LocalDiscussion>;

template class Infector<ContactLogMode::Id::All, false, NoLocalInformation>;
template class Infector<ContactLogMode::Id::All, false, LocalDiscussion>;
template class Infector<ContactLogMode::Id::All, true, NoLocalInformation>;
template class Infector<ContactLogMode::Id::All, true, LocalDiscussion>;

template class Infector<ContactLogMode::Id::Susceptibles, false, NoLocalInformation>;
template class Infector<ContactLogMode::Id::Susceptibles, false, LocalDiscussion>;
template class Infector<ContactLogMode::Id::Susceptibles, true, NoLocalInformation>;
template class Infector<ContactLogMode::Id::Susceptibles, true, LocalDiscussion>;

} // namespace stride
