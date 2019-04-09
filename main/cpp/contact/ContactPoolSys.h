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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Container for the contact pools of various type (household, work, ...)..
 */

#pragma once

#include "ContactType.h"
#include "IdSubscriptArray.h"
#include "contact/ContactPool.h"
#include "util/SegmentedVector.h"

namespace stride {

/**
 * ContactPoolSys contains for each of the type of pools (Household, K12School, ...)
 * a vector of with all of the contact pools of the type.
 * The ContactPoolSys container is an std::array extended to be subscriptable
 * with the enum class of the pool type Ids.
 */
class ContactPoolSys
{
public:
        /// Empty system.
        ContactPoolSys();

        /// Create a new contact pool of a given type.
        ContactPool* CreateContactPool(ContactType::Id typeId);

        /// Templated version of @CreateContactPool for use when type id is fixed.
        /// \tparam T   One of the ContactType::Id's (Household, K12 School, ...).
        /// \return     Pointer to the newly created ContactPool.
        template <ContactType::Id T>
        ContactPool* CreateContactPool()
        {
                return m_sys[T].emplace_back(m_currentContactPoolId[T]++, T);
        }

        /// Access through const reference to ContactPools of type 'id'.
        /// \param id   ContactType::Id of pools container you want to access.
        /// \return     The requested reference.
        const util::SegmentedVector<ContactPool>& CRefPools(ContactType::Id id) const { return m_sys[id]; }

        /// Templated version of @CRefPools for use when the type id is fixed
        /// \tparam T   ContactType::Id of pools container you want to access.
        /// \return     The requested reference.
        template <ContactType::Id T>
        const util::SegmentedVector<ContactPool>& CRefPools() const
        {
                return m_sys[T];
        }

private:
        /// /// Access through non-const reference to ContactPools of type 'id'.
        /// \param id   ContactType::Id of pools container you want to access.
        /// \return     The requested reference.
        util::SegmentedVector<ContactPool>& RefPools(ContactType::Id id) { return m_sys[id]; }

        friend class DefaultPopBuilder;
        friend class Sim;

private:
        /// The contact pool counters (one per type id) for assigning pool UIDs. Counters
        /// generate a non zero UID that's unique per type of pool, so <type, UID> uniquely
        /// detemines the pool. UID zero means 'NA" e.g. wor[lace UID for a K12school student
        /// will be zero. As a defensive measure, the ContactPoolSys gets initialized with
        /// (for each type) an empty pool in the vecotor storing the contact pools. As a
        /// consequence, one has:
        /// if UID != 0 then ContactPoolSys[type][UID].GetId() == UID for all type
        /// the index in the vector with pools is identical to the pool's UID.
        ContactType::IdSubscriptArray<unsigned int> m_currentContactPoolId;

        /// Pool system container: array that is subscriptable with ContactType::Ids and for
        /// each Id contains a SegmentedVector with the ContactPools for that ContactType::Id.
        /// We use the SegmentedVector not to run in re-allocations and to be able to use
        /// pointers into the SegmentedVector.
        ContactType::IdSubscriptArray<util::SegmentedVector<ContactPool>> m_sys;
};

} // namespace stride
