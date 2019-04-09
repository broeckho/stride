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

#include <algorithm>

namespace stride {

using namespace std;

ContactPool::ContactPool(unsigned int poolId, ContactType::Id type)
    : m_index_immune(0), m_pool_id(poolId), m_pool_type(type), m_members()
{
}

void ContactPool::AddMember(Person* p)
{
        m_members.emplace_back(p);
        m_index_immune++;
}

unsigned int ContactPool::GetInfectedCount() const
{
        unsigned int infected = 0;

        for (stride::Person* person : m_members) {
                if (person->GetHealth().IsInfected()) {
                        infected++;
                }
        }
        return infected;
}

std::tuple<bool, unsigned int> ContactPool::SortMembers()
{
        bool         infectious_cases = false;
        unsigned int num_cases        = 0;

        for (size_t i_member = 0; i_member < m_index_immune; i_member++) {
                // if immune, move to back
                if (m_members[i_member]->GetHealth().IsImmune()) {
                        bool         swapped   = false;
                        unsigned int new_place = m_index_immune - 1;
                        m_index_immune--;
                        while (!swapped && new_place > i_member) {
                                if (m_members[new_place]->GetHealth().IsImmune()) {
                                        m_index_immune--;
                                        new_place--;
                                } else {
                                        swap(m_members[i_member], m_members[new_place]);
                                        swapped = true;
                                }
                        }
                }
                // else, if not susceptible, move to front
                else if (!m_members[i_member]->GetHealth().IsSusceptible()) {
                        if (!infectious_cases && m_members[i_member]->GetHealth().IsInfectious()) {
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

} // namespace stride
