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
 * Header for the core ContactPool class.
 */

#pragma once

#include "contact/ContactLogMode.h"
#include "contact/ContactType.h"

#include <tuple>
#include <vector>

namespace stride {

class Person;

/**
 * A group of Persons that potentially have contacts with one another.
 * We do not expose the vector that stores pool members because
 * adding & sorting it takes some care.
 */
class ContactPool
{
public:
        /// Initializing constructor.
        ContactPool(unsigned int poolId, ContactType::Id type);

        /// Default will do.
        ~ContactPool() = default;

        /// Add the given Person.
        void AddMember(Person* p);

        /// Get the pool id
        unsigned int GetId() const { return m_pool_id; }

        /// Get Infected count
        unsigned int GetInfectedCount() const;

        /// Get the entire pool of members.
        const std::vector<Person*>& GetPool() const { return m_members; }

        /// Get the type of ContactPool, used for logging and tests
        ContactType::Id GetType() const { return m_pool_type; }

public:
        // To iterate over the members.
        using iterator = std::vector<stride::Person*>::iterator;

        /// Iterator to first person
        iterator begin() { return m_members.begin(); }

        /// Iterator to end of persons
        iterator end() { return m_members.end(); }

        /// Gets current size of Location storage.
        size_t size() const { return m_members.size(); }

        /// Gets a Person by index, doesn't performs a range check.
        Person* const& operator[](size_t index) const { return m_members[index]; }

private:
        /// Sort w.r.t. health status: order: exposed/infected/recovered, susceptible, immune.
        std::tuple<bool, unsigned int> SortMembers();

        /// Calculates contacts and transmissions; accesses private methods and data.
        template <ContactLogMode::Id LL, bool TIC, bool TO>
        friend class Infector;

private:
        unsigned int         m_index_immune; ///< Index of the first immune member in the ContactPool.
        unsigned int         m_pool_id;      ///< The ID of the ContactPool (for logging purposes).
        ContactType::Id      m_pool_type;    ///< The type of the ContactPool (for logging and testing purposes).
        std::vector<Person*> m_members;      ///< Pointers to contactpool members (raw pointers intentional).
};

} // namespace stride
