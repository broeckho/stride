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
 * ContactPoolSys contains for each of the type of pools (household, school, ...)
 * a vector of with all of the contcact pools of the type.
 * The ContactPoolSys container is an std::array extended to be subscriptable
 * with the enum class of the pool types.
 */
class ContactPoolSys : public ContactType::IdSubscriptArray<util::SegmentedVector<ContactPool>>
{
public:
        /// Empty system.
        ContactPoolSys();

        /// Add a new contact pool of a given type
        ContactPool* CreateContactPool(ContactType::Id typeId);

private:
        /// The contact pool counters (one per type of pool) for assigning pool IDs. Counters
        /// generate a non zero UID that's unique per type of pool, so <type, UID> uniquely
        /// detemines the pool. UID zero means 'NA" e.g. wor[lace UID for a K12school student
        /// will be zero. As a defensive measure, the ContactPoolSys gets initialized with
        /// (for each type) an empty pool in the vecotor storing the contact pools. As a
        /// consequence, one has:
        /// if UID != 0 then ContactPoolSys[type][UID].GetId() == UID for all type
        /// the index in the vector with pools is identical to the pool's UID.
        ContactType::IdSubscriptArray<unsigned int> m_currentContactPoolId;
};

} // namespace stride
