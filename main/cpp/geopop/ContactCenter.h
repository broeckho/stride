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
}

namespace geopop {

class GeoGrid;
class GeoGridConfig;

/**
 * A ContactCenter contains multiple ContactPools
 */
class ContactCenter
{
public:
        /// Construct ContactCenter with assigned ID.
        explicit ContactCenter(unsigned int id) : m_pools(), m_id(id) {}

        /// Create ContactPools in the GeoGrid and register them with the ContactCenter.
        virtual void Fill(const GeoGridConfig& geoGridConfig, const std::shared_ptr<GeoGrid>& geoGrid) = 0;

        /// Return the ID of this ContactCenter.
        unsigned int GetId() const { return m_id; }

        /// Get the ContactPools container.
        const std::vector<stride::ContactPool*>& GetPools() const { return m_pools; }

        /// Get a count of total population (first) and total number of infections (second).
        std::pair<unsigned int, unsigned int> GetPopulationAndInfectedCount() const;

        /// Get the the type of ContactPools this ContactCenter contains.
        virtual stride::ContactType::Id GetContactPoolType() const = 0;

        /// Register a ContactPool with this ContactCenter.
        void RegisterPool(stride::ContactPool* pool) { m_pools.emplace_back(pool); }

        /// Default destructor, but virtual
        virtual ~ContactCenter() = default;

public:
        /// Allow range-based iteration over pools in contact center.
        using iterator = std::vector<stride::ContactPool*>::iterator;

        /// Allow range-based iteration over pools in contact center.
        iterator begin() { return m_pools.begin(); }

        /// Allow range-based iteration over pools in contact center.
        iterator end() { return m_pools.end(); }

protected:
        std::vector<stride::ContactPool*> m_pools; ///< ConatctPools for this ContactCenter.
        unsigned int                      m_id;    ///< The Id of this ContactCenter.
};

} // namespace geopop
