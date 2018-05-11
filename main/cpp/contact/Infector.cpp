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
#include "pool/ContactPool.h"
#include "pop/Person.h"

using namespace std;

namespace {

/// Primary LOG_POLICY policy, implements LogMode::None.
/// \tparam LL
template <ContactLogMode::Id LL>
class LOG_POLICY
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                            unsigned short int)
        {
        }

        static void Trans(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                          unsigned short int)
        {
        }
};

/// Specialized LOG_POLICY policy LogMode::Transmissions.
template <>
class LOG_POLICY<ContactLogMode::Id::Transmissions>
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                            unsigned short int)
        {
        }

        static void Trans(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                          ContactPoolType::Id type, unsigned short int sim_day)
        {
                logger->info("[TRAN] {} {} {} {}", p1->GetId(), p2->GetId(), ToString(type), sim_day);
        }
};

/// Specialized LOG_POLICY policy LogMode::All.
template <>
class LOG_POLICY<ContactLogMode::Id::All>
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                            ContactPoolType::Id type, unsigned short int sim_day)
        {
                if (p1->IsSurveyParticipant()) {
                        logger->info("[CONT] {} {} {} {} {} {} {} {} {}", p1->GetId(), p1->GetAge(), p2->GetAge(),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::Household),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::School),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::Work),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::PrimaryCommunity),
                                     static_cast<unsigned int>(type == ContactPoolType::Id::SecondaryCommunity),
                                     sim_day);
                }
        }

        static void Trans(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                          ContactPoolType::Id type, unsigned short int sim_day)
        {
                logger->info("[TRAN] {} {} {} {}", p1->GetId(), p2->GetId(), ToString(type), sim_day);
        }
};

/// Specialized LOG_POLICY policy LogMode::Susceptibles.
template <>
class LOG_POLICY<ContactLogMode::Id::Susceptibles>
{
public:
        static void Contact(const std::shared_ptr<spdlog::logger>& logger, const Person* p1, const Person* p2,
                            ContactPoolType::Id, unsigned short int)
        {
                if (p1->IsSurveyParticipant() && p1->GetHealth().IsSusceptible() && p2->GetHealth().IsSusceptible()) {
                        logger->info("[CONT] {} {}", p1->GetId(), p2->GetId());
                }
        }

        static void Trans(const std::shared_ptr<spdlog::logger>&, const Person*, const Person*, ContactPoolType::Id,
                          unsigned short int)
        {
        }
};

}

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
                                      const TransmissionProfile& transProfile, ContactHandler& cHandler,
                                      unsigned short int simDay, shared_ptr<spdlog::logger> cLogger)
{
        using LP = LOG_POLICY<LL>;

        // set up some stuff
        const auto  pType    = pool.m_pool_type;
        const auto& pMembers = pool.m_members;
        const auto  pSize    = pMembers.size();
        const auto  tRate    = transProfile.GetRate();

        // check all contacts
        for (size_t i_person1 = 0; i_person1 < pSize; i_person1++) {
                // check if member is present today
                const auto p1 = pMembers[i_person1];
                if (p1->IsInPool(pType)) {
                        const double c_rate = GetContactRate(profile, p1, pSize);
                        // loop over possible contacts (contacts can be initiated by each member)
                        for (size_t i_person2 = 0; i_person2 < pSize; i_person2++) {
                                // check if not the same person
                                if (i_person1 != i_person2) {
                                        // check if member is present today
                                        const auto p2 = pMembers[i_person2];
                                        if (p2->IsInPool(pType)) {
                                                // check for contact
                                                if (cHandler.HasContact(c_rate)) {
                                                        // log contact if person 1 is participating in survey
                                                        LP::Contact(cLogger, p1, p2, pType, simDay);
                                                        // log contact if person 2 is participating in survey
                                                        LP::Contact(cLogger, p2, p1, pType, simDay);

                                                        // exchange info about health state & beliefs
                                                        LIP::Update(p1, p2);

                                                        // transmission & infection.
                                                        if (cHandler.HasTransmission(tRate)) {
                                                                auto& h1 = p1->GetHealth();
                                                                auto& h2 = p2->GetHealth();
                                                                // No secondary infections with TIC; just mark
                                                                // p2 as being recovered
                                                                if (h1.IsInfectious() && h2.IsSusceptible()) {
                                                                        LP::Trans(cLogger, p1, p2, pType, simDay);
                                                                        h2.StartInfection();
                                                                        if (TIC)
                                                                                h2.StopInfection();
                                                                } else if (h2.IsInfectious() && h1.IsSusceptible()) {
                                                                        LP::Trans(cLogger, p2, p1, pType, simDay);
                                                                        h1.StartInfection();
                                                                        if (TIC)
                                                                                h1.StopInfection();
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
// Time optimized implementation for NoLocalInformationPolicy in
// combination with None || Transmission logging.
//-------------------------------------------------------------------------------------------
template <ContactLogMode::Id LL, bool TIC>
void Infector<LL, TIC, NoLocalInformation, true>::Exec(ContactPool& pool, const AgeContactProfile& profile,
                                                       const TransmissionProfile& transProfile,
                                                       ContactHandler& cHandler, unsigned short int simDay,
                                                       shared_ptr<spdlog::logger> cLogger)
{
        using LP = LOG_POLICY<LL>;

        // check for infected members and sort
        bool   infectious_cases;
        size_t num_cases;
        tie(infectious_cases, num_cases) = pool.SortMembers();

        if (!infectious_cases) {
                return;
        }

        // set up some stuff
        const auto  c_type    = pool.m_pool_type;
        const auto  c_immune  = pool.m_index_immune;
        const auto& c_members = pool.m_members;
        const auto  c_size    = c_members.size();
        const auto  t_rate    = transProfile.GetRate();

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
                                                if (cHandler.HasContactAndTransmission(c_rate_p1, t_rate) ||
                                                    cHandler.HasContactAndTransmission(c_rate_p2, t_rate)) {
                                                        auto& h2 = p2->GetHealth();
                                                        if (h1.IsInfectious() && h2.IsSusceptible()) {
                                                                h2.StartInfection();
                                                                // No secondary infections with TIC; just mark
                                                                // p2 as being recovered
                                                                if (TIC)
                                                                        h2.StopInfection();
                                                                LP::Trans(cLogger, p1, p2, c_type, simDay);
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
