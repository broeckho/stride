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
 * Implementation for the core Cluster class.
 */

#include "Cluster.h"

namespace stride {

using namespace std;

array<ContactProfile, NumOfClusterTypes()> Cluster::g_profiles;

Cluster::Cluster(std::size_t cluster_id, ClusterType cluster_type)
    : m_cluster_id(cluster_id), m_cluster_type(cluster_type), m_index_immune(0),
      m_profile(g_profiles.at(ToSizeType(m_cluster_type)))
{
}

void Cluster::AddContactProfile(ClusterType cluster_type, const ContactProfile& profile)
{
	g_profiles.at(ToSizeType(cluster_type)) = profile;
}

void Cluster::AddPerson(Person* p)
{
	m_members.emplace_back(std::make_pair(p, true));
	m_index_immune++;
}

double Cluster::GetContactRate(const Person* p) const
{
	const auto i {ToSizeType(m_cluster_type)};
	const double reference_num_contacts = g_profiles.at(i)[EffectiveAge(static_cast<unsigned int>(p->GetAge()))];
	const double potential_num_contacts = (m_members.size() - 1);

	double individual_contact_rate = reference_num_contacts / potential_num_contacts;
	if (individual_contact_rate >= 1) {
		individual_contact_rate = 0.999;
	}
	// given that contacts are reciprocal, one needs to make only half of his/her contacts himself/herself
	individual_contact_rate = individual_contact_rate / 2;

	// take into account that contacts are not directed
	// contact probability for 1=>2 and 2=>1 = indiv_cnt_rate*indiv_cnt_rate
	individual_contact_rate += (individual_contact_rate * individual_contact_rate);

	return individual_contact_rate;
}

tuple<bool, size_t> Cluster::SortMembers()
{
	bool infectious_cases = false;
	size_t num_cases = 0;

	for (size_t i_member = 0; i_member < m_index_immune; i_member++) {
		// if immune, move to back
		if (m_members[i_member].first->GetHealth().IsImmune()) {
			bool swapped = false;
			size_t new_place = m_index_immune - 1;
			m_index_immune--;
			while (!swapped && new_place > i_member) {
				if (m_members[new_place].first->GetHealth().IsImmune()) {
					m_index_immune--;
					new_place--;
				} else {
					swap(m_members[i_member], m_members[new_place]);
					swapped = true;
				}
			}
		}
		// else, if not susceptible, move to front
		else if (!m_members[i_member].first->GetHealth().IsSusceptible()) {
			if (!infectious_cases && m_members[i_member].first->GetHealth().IsInfectious()) {
				infectious_cases = true;
			}
			if (i_member > num_cases) {
				swap(m_members[i_member], m_members[num_cases]);
			}
			num_cases++;
		}
	}
	return std::make_tuple(infectious_cases, num_cases);
}

void Cluster::UpdateMemberPresence()
{
	for (auto& member : m_members) {
		member.second = member.first->IsInCluster(m_cluster_type);
	}
}

} // end_of_namespace
