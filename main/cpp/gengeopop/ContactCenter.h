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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#pragma once
#include "GeoGrid.h"

namespace gengeopop {

class GeoGridConfig;

/// A ContactCenter contains multiple ContactPools
class ContactCenter
{
public:
        /// Allow range-based iteration
        using iterator = std::vector<stride::ContactPool*>::iterator;

        /// Constructor
        explicit ContactCenter(unsigned int id);

        /// Get the name of the type of contact center (e.g. College)
        virtual std::string GetType() const = 0;

        /// Get the size of a pool
        virtual unsigned int GetPoolSize() const = 0;

        /// Get the maximal number of pools for this center
        virtual unsigned int GetMaxPools() const = 0;

        /// Apply this ContactCenter to the GeoGrid
        virtual void Fill(const std::shared_ptr<GeoGrid>& geoGrid) = 0;

        /// Get the ID back
        unsigned int GetId() const;

        /// Get a count of total population (first) and total number of infections (second)
        std::pair<unsigned int, unsigned int> GetPopulationAndInfectedCount() const;

        /// Add a ContactPool
        void AddPool(stride::ContactPool* pool);

        /// Get the pools
        const std::vector<stride::ContactPool*>& GetPools() const;

        /// Allow range-based iteration
        iterator begin();
        /// Allow range-based iteration
        iterator end();

        /// Default destructor, but virtual
        virtual ~ContactCenter() = default;

protected:
        std::vector<stride::ContactPool*> m_pools; ///< Our pools
        unsigned int                      m_id;    ///< The id of this ContactCenter
};

} // namespace gengeopop
