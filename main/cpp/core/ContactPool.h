#pragma once
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

#include "core/ContactPoolType.h"
#include "core/ContactProfile.h"
#include "core/ContactProfiles.h"
#include "core/LogMode.h"
#include "pop/Person.h"

#include <array>
#include <vector>

namespace stride {

/**
 * Represents a group of Persons for potential contacts.
 */
class ContactPool
{
public:
        /// Initializing constructor
        ContactPool(std::size_t pool_id, ContactPoolType::Id type, const ContactProfiles& profiles);

        /// No copying: too big.
        ContactPool(const ContactPool&) = delete;

        /// Moving is ok.
        ContactPool(ContactPool&&) = default;

        // No assignment: too big.
        ContactPool& operator=(const ContactPool&) = delete;

        /// Add the given Person.
        void AddMember(Person* p);

        /// Get basic contact rate.
        double GetContactRate(const Person* p) const;

        /// Get size
        unsigned int GetSize() const;

        /// Get member at index
        Person* GetMember(unsigned int index) const;

private:
        /// Sort members w.r.t. health status.
        /// Sort order: exposed/infected/recovered, susceptible, immune).
        std::tuple<bool, size_t> SortMembers();

        /// Infector calculates contacts and transmissions.
        template <LogMode::Id LL, bool TIC, typename LIP, bool TO>
        friend class Infector;

        /// Calculate which members are present on the current day.
        void UpdateMemberPresence();

private:
        std::size_t                           m_pool_id;      ///< The ID of the Cluster (for logging purposes).
        ContactPoolType::Id                   m_pool_type;    ///< The type of the Cluster (for logging purposes).
        std::size_t                           m_index_immune; ///< Index of the first immune member in the Cluster.
        std::vector<std::pair<Person*, bool>> m_members;      ///< Container with pointers to Cluster members.
        const ContactProfile&                 m_profile;      ///< Contact pattern.
};

} // namespace stride
