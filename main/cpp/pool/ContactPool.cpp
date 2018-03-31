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
 * Implementation for the core ContcatPool class.
 */

#include "ContactPool.h"

#include "pop/Age.h"
#include "pop/Person.h"

namespace stride {

using namespace std;

ContactPool::ContactPool(std::size_t pool_id, ContactPoolType::Id type)
    : m_pool_id(pool_id), m_pool_type(type), m_index_immune(0), m_members()
{
}

void ContactPool::AddMember(const Person* p)
{
        m_members.emplace_back(std::make_pair(const_cast<Person*>(p), true));
        m_index_immune++;
}

size_t ContactPool::GetSize() const { return m_members.size(); }

Person* ContactPool::GetMember(unsigned int index) const { return m_members[index].first; }

const std::vector<std::pair<Person*, bool>>& ContactPool::GetPool() const { return m_members; }

std::tuple<bool, size_t> ContactPool::SortMembers()
{
        bool   infectious_cases = false;
        size_t num_cases        = 0;

        for (size_t i_member = 0; i_member < m_index_immune; i_member++) {
                // if immune, move to back
                if (m_members[i_member].first->GetHealth().IsImmune()) {
                        bool   swapped   = false;
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

void ContactPool::UpdateMemberPresence()
{
        for (auto& member : m_members) {
                member.second = member.first->IsInPool(m_pool_type);
        }
}

} // namespace stride
