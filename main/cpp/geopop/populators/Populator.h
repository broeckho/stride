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
#include "util/RnMan.h"

#include <spdlog/logger.h>

namespace stride {
class ContactPool;
}

namespace geopop {

class GeoGrid;
class GeoGridConfig;
class Location;

/**
 * Interface for populators. They generate some data and apply it to the GeoGrid.
 */
class Populator
{
public:
        /// Construct with a RnMan and a logger.
        explicit Populator(stride::util::RnMan& rnMan, std::shared_ptr<spdlog::logger> logger = nullptr);

        /// Virtual destructor for inheritance.
        virtual ~Populator() = default;

        /// Populate the given geogrid for pool type (fixed in implementation).
        virtual void Apply(GeoGrid& geogrid, const GeoGridConfig& geoGridConfig) = 0;

protected:
        /// Find contactpools in startRadius (in km) around start and, if none are found, double
        /// the radius and search again until the radius gets infinite. May return an empty vector
        /// when there are no pools to be found.
        std::vector<stride::ContactPool*> GetNearbyPools(stride::ContactType::Id id, const GeoGrid& geoGrid,
                                                         const Location& start, double startRadius = 10.0) const;

        /// Binary selection corresponding to fraction% true en (1-fraction)% false.
        bool MakeChoice(double fraction);

protected:
        stride::util::RnMan&            m_rn_man; ///< RnManager used by populators.
        std::shared_ptr<spdlog::logger> m_logger; ///< Logger used by populators.
};

} // namespace geopop
