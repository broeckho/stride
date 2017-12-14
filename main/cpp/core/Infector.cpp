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

#include "core/Cluster.h"

#include <spdlog/spdlog.h>

namespace stride {

using namespace std;

/**
 * Primary R0_POLICY: do nothing i.e. track all cases.
 */
template <bool track_index_case>
class R0_POLICY
{
public:
        static void Execute(Person* p) {}
};

/**
 * Specialized R0_POLICY: track only the index case.
 */
template <>
class R0_POLICY<true>
{
public:
        static void Execute(Person* p) { p->GetHealth().StopInfection(); }
};

/**
 * Primary LOG_POLICY policy, implements LogMode::None.
 */
template <LogMode log_level>
class LOG_POLICY
{
public:
        static void Execute(const shared_ptr<spdlog::logger>& logger, Person* p1, Person* p2, ClusterType cluster_type,
                            const shared_ptr<const Calendar>& environ)
        {
        }
};

/**
 * Specialized LOG_POLICY policy LogMode::Transmissions.
 */
template <>
class LOG_POLICY<LogMode::Transmissions>
{
public:
        static void Execute(const shared_ptr<spdlog::logger>& logger, Person* p1, Person* p2, ClusterType cluster_type,
                            const shared_ptr<const Calendar>& environ)
        {
                logger->info("[TRAN] {} {} {} {}", p1->GetId(), p2->GetId(), ToString(cluster_type),
                             environ->GetSimulationDay());
        }
};

/**
 * Specialized LOG_POLICY policy LogMode::Contacts.
 */
template <>
class LOG_POLICY<LogMode::Contacts>
{
public:
        static void Execute(const shared_ptr<spdlog::logger>& logger, Person* p1, Person* p2, ClusterType cluster_type,
                            const shared_ptr<const Calendar>& calendar)
        {
                const auto home = (cluster_type == ClusterType::Household);
                const auto work = (cluster_type == ClusterType::Work);
                const auto school = (cluster_type == ClusterType::School);
                const auto primary_community = (cluster_type == ClusterType::PrimaryCommunity);
                const auto secundary_community = (cluster_type == ClusterType::SecondaryCommunity);

                logger->info("[CONT] {} {} {} {} {} {} {} {} {}", p1->GetId(), p1->GetAge(), p2->GetAge(),
                             static_cast<unsigned int>(home), static_cast<unsigned int>(school),
                             static_cast<unsigned int>(work), static_cast<unsigned int>(primary_community),
                             static_cast<unsigned int>(secundary_community), calendar->GetSimulationDay());
        }
};

//--------------------------------------------------------------------------
// Definition for primary template covers the situation for
// LogMode::None & LogMode::Transmissions, both with
// track_index_case false and true.
// And every local information policy except NoLocalInformation
//--------------------------------------------------------------------------
template <LogMode log_level, bool track_index_case, typename local_information_policy>
void Infector<log_level, track_index_case, local_information_policy>::Execute(Cluster& cluster,
                                                                              DiseaseProfile disease_profile,
                                                                              RngHandler& contact_handler,
                                                                              shared_ptr<const Calendar> calendar)
{
        cluster.UpdateMemberPresence();

        // set up some stuff
        auto logger = spdlog::get("contact_logger");
        const auto c_type = cluster.m_cluster_type;
        const auto& c_members = cluster.m_members;
        const auto transmission_rate = disease_profile.GetTransmissionRate();

        // check all contacts
        for (size_t i_person1 = 0; i_person1 < c_members.size(); i_person1++) {
                // check if member is present today
                if (c_members[i_person1].second) {
                        auto p1 = c_members[i_person1].first;

                        const double contact_rate = cluster.GetContactRate(p1);
                        // loop over possible contacts (contacts can be initiated by each member)
                        for (size_t i_person2 = 0; i_person2 < c_members.size(); i_person2++) {
                                // check if member is present today
                                if (c_members[i_person2].second) {
                                        auto p2 = c_members[i_person2].first;

                                        // check for contact
                                        if (contact_handler.HasContact(contact_rate)) {
                                                // exchange information about health state & beliefs
                                                local_information_policy::Update(p1, p2);

                                                bool transmission = contact_handler.HasTransmission(transmission_rate);
                                                if (transmission) {
                                                        if (p1->GetHealth().IsInfectious() &&
                                                            p2->GetHealth().IsSusceptible()) {
                                                                LOG_POLICY<log_level>::Execute(logger, p1, p2, c_type,
                                                                                               calendar);
                                                                p2->GetHealth().StartInfection();
                                                                R0_POLICY<track_index_case>::Execute(p2);
                                                        } else if (p2->GetHealth().IsInfectious() &&
                                                                   p1->GetHealth().IsSusceptible()) {
                                                                LOG_POLICY<log_level>::Execute(logger, p2, p1, c_type,
                                                                                               calendar);
                                                                p1->GetHealth().StartInfection();
                                                                R0_POLICY<track_index_case>::Execute(p1);
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
}

//-------------------------------------------------------------------------------------------
// Definition of partial specialization for
// LocalInformationPolicy:NoLocalInformation.
//-------------------------------------------------------------------------------------------
template <LogMode log_level, bool track_index_case>
void Infector<log_level, track_index_case, NoLocalInformation>::Execute(Cluster& cluster,
                                                                        DiseaseProfile disease_profile,
                                                                        RngHandler& contact_handler,
                                                                        shared_ptr<const Calendar> calendar)
{

        // check if the cluster has infected members and sort
        bool infectious_cases;
        size_t num_cases;
        tie(infectious_cases, num_cases) = cluster.SortMembers();

        if (infectious_cases) {
                cluster.UpdateMemberPresence();

                // set up some stuff
                auto logger = spdlog::get("contact_logger");
                const auto c_type = cluster.m_cluster_type;
                const auto c_immune = cluster.m_index_immune;
                const auto& c_members = cluster.m_members;
                const auto transmission_rate = disease_profile.GetTransmissionRate();

                // match infectious and susceptible members, skip last part (immune members)
                for (size_t i_infected = 0; i_infected < num_cases; i_infected++) {
                        // check if member is present today
                        if (c_members[i_infected].second) {
                                const auto p1 = c_members[i_infected].first;

                                if (p1->GetHealth().IsInfectious()) {
                                        const double contact_rate_p1 = cluster.GetContactRate(p1);

                                        // loop over possible susceptible contacts
                                        for (size_t i_contact = num_cases; i_contact < c_immune; i_contact++) {

                                                // check if member is present today
                                                if (c_members[i_contact].second) {

                                                        auto p2 = c_members[i_contact].first;
                                                        const double contact_rate_p2 = cluster.GetContactRate(p2);

                                                        // check for contact and transmission
                                                        // p1 => p2 OR p2 => p1
                                                        if (contact_handler.HasContactAndTransmission(
                                                                contact_rate_p1, transmission_rate) ||
                                                            contact_handler.HasContactAndTransmission(
                                                                contact_rate_p2, transmission_rate)) {

                                                                if (p1->GetHealth().IsInfectious() &&
                                                                    p2->GetHealth().IsSusceptible()) {

                                                                        p2->GetHealth().StartInfection();
                                                                        R0_POLICY<track_index_case>::Execute(p2);
                                                                        LOG_POLICY<log_level>::Execute(
                                                                            logger, p1, p2, c_type, calendar);
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
// Definition of partial specialization for LogMode::Contacts and
// NoLocalInformation policy.
//-------------------------------------------------------------------------------------------
template <bool track_index_case>
void Infector<LogMode::Contacts, track_index_case, NoLocalInformation>::Execute(Cluster& cluster,
                                                                                DiseaseProfile disease_profile,
                                                                                RngHandler& contact_handler,
                                                                                shared_ptr<const Calendar> calendar)
{
        cluster.UpdateMemberPresence();

        // set up some stuff
        auto logger = spdlog::get("contact_logger");
        const auto c_type = cluster.m_cluster_type;
        const auto& c_members = cluster.m_members;
        const auto transmission_rate = disease_profile.GetTransmissionRate();

        // check all contacts
        for (size_t i_person1 = 0; i_person1 < c_members.size(); i_person1++) {
                // check if member is present today
                if (c_members[i_person1].second) {
                        auto p1 = c_members[i_person1].first;
                        const double contact_rate = cluster.GetContactRate(p1);
                        // loop over possible contacts
                        for (size_t i_person2 = 0; i_person2 < c_members.size(); i_person2++) {
                                // check if possible contact is present today
                                if (i_person2 != i_person1 && c_members[i_person2].second) {
                                        auto p2 = c_members[i_person2].first;
                                        // check for effective contact
                                        if (contact_handler.HasContact(contact_rate)) {

                                                // log contact, if person 1 is participating in survey
                                                if (c_members[i_person1].first->IsParticipatingInSurvey()) {
                                                        LOG_POLICY<LogMode::Contacts>::Execute(logger, p1, p2, c_type,
                                                                                               calendar);
                                                }
                                                // log contact, if person 2 is participating in survey
                                                if (c_members[i_person2].first->IsParticipatingInSurvey()) {
                                                        LOG_POLICY<LogMode::Contacts>::Execute(logger, p2, p1, c_type,
                                                                                               calendar);
                                                }

                                                // given the contact, check for transmission
                                                bool transmission = contact_handler.HasTransmission(transmission_rate);
                                                if (transmission) {
                                                        if (p1->GetHealth().IsInfectious() &&
                                                            p2->GetHealth().IsSusceptible()) {
                                                                p2->GetHealth().StartInfection();
                                                                R0_POLICY<track_index_case>::Execute(p2);
                                                        } else if (p2->GetHealth().IsInfectious() &&
                                                                   p1->GetHealth().IsSusceptible()) {
                                                                p1->GetHealth().StartInfection();
                                                                R0_POLICY<track_index_case>::Execute(p1);
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
template class Infector<LogMode::None, false, NoLocalInformation>;
template class Infector<LogMode::None, false, LocalDiscussion>;
template class Infector<LogMode::None, true, NoLocalInformation>;
template class Infector<LogMode::None, true, LocalDiscussion>;

template class Infector<LogMode::Transmissions, false, NoLocalInformation>;
template class Infector<LogMode::Transmissions, false, LocalDiscussion>;
template class Infector<LogMode::Transmissions, true, NoLocalInformation>;
template class Infector<LogMode::Transmissions, true, LocalDiscussion>;

template class Infector<LogMode::Contacts, false, NoLocalInformation>;
template class Infector<LogMode::Contacts, false, LocalDiscussion>;
template class Infector<LogMode::Contacts, true, NoLocalInformation>;
template class Infector<LogMode::Contacts, true, LocalDiscussion>;

} // end_of_namespace
