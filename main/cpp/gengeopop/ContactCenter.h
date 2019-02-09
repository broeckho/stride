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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#pragma once

//#include "GeoGrid.h"

#include "pool/ContactPool.h"

#include <memory>
#include <vector>

namespace gengeopop {

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

        /// Add a ContactPool.
        void AddPool(stride::ContactPool* pool) { m_pools.emplace_back(pool); }

        /// Apply this ContactCenter to the GeoGrid.
        virtual void Fill(const std::shared_ptr<GeoGrid>& geoGrid) = 0;

        /// Return the ID.
        unsigned int GetId() const { return m_id; }

        /// Get the maximum number of pools for this contact center.
        virtual unsigned int GetMaxPools() const = 0;

        /// Get the pools container.
        const std::vector<stride::ContactPool*>& GetPools() const { return m_pools; }

        /// Get the size (on average) of a pool for this type of contact center.
        virtual unsigned int GetPoolSize() const = 0;

        /// Get a count of total population (first) and total number of infections (second).
        std::pair<unsigned int, unsigned int> GetPopulationAndInfectedCount() const;

        /// Get the name of the type of contact center (e.g. College)
        virtual std::string GetType() const = 0;

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
        std::vector<stride::ContactPool*> m_pools; ///< Our pools
        unsigned int                      m_id;    ///< The id of this ContactCenter
};

} // namespace gengeopop
