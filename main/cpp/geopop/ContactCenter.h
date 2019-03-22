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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "contact/ContactType.h"

#include <memory>
#include <vector>

namespace stride {
class ContactPool;
class Population;
} // namespace stride

namespace geopop {

class GeoGridConfig;

/**
 * A ContactCenter represents a number of ContactPools of same ContactType::Id
 * that are related. Examples are classes (Pools with ID value K12School) vs
 * school (ContactCenter K12School) or teams (Pools with Id value Workplace)
 * vs company (ContactCenter WorkPlace).
 */
class ContactCenter
{
public:
        /// Construct ContactCenter with assigned ID.
        explicit ContactCenter(unsigned int id, stride::ContactType::Id typeId) : m_pools(), m_id(id), m_type_id(typeId)
        {
        }

        /// Default destructor, but virtual
        virtual ~ContactCenter() = default;

        /// Get the the type of ContactPools this ContactCenter contains.
        stride::ContactType::Id GetContactPoolType() const { return m_type_id; };

        /// Return the ID of this ContactCenter.
        unsigned int GetId() const { return m_id; }

        /// Get a count of total population (first) and total number of infections (second).
        std::pair<unsigned int, unsigned int> GetPopulationAndInfectedCount() const;

        /// Register a ContactPool with this ContactCenter.
        void RegisterPool(stride::ContactPool* pool) { m_pools.emplace_back(pool); }

public:
        using iterator       = std::vector<stride::ContactPool*>::iterator;
        using const_iterator = std::vector<stride::ContactPool*>::const_iterator;

        /// Iterator to first Location.
        iterator begin() { return m_pools.begin(); }

        /// Iterator to the end of the Location storage.
        iterator end() { return m_pools.end(); }

        /// Const Iterator to first Location.
        const_iterator cbegin() const { return m_pools.cbegin(); }

        /// Const iterator to the end of the Location storage.
        const_iterator cend() const { return m_pools.cend(); }

        /// Gets a ContactPool by index, doesn't performs a range check.
        stride::ContactPool* const& operator[](size_t index) const { return m_pools[index]; }

        /// Gets current size of Location storage.
        size_t size() const { return m_pools.size(); }

protected:
        std::vector<stride::ContactPool*> m_pools;   ///< ContactPools for this ContactCenter.
        unsigned int                      m_id;      ///< The Id of this ContactCenter.
        stride::ContactType::Id           m_type_id; ///< The ContactTYpe Id for this center.
};

} // namespace geopop
