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

#include "Populator.h"

#include "geopop/GeoGridConfig.h"
#include "pop/Person.h"

namespace geopop {

/**
 * Populate the workplaces.
 */

class WorkplacePopulator : public Populator
{
public:
        /// Constructor
        explicit WorkplacePopulator(stride::util::RnMan& rnMan, std::shared_ptr<spdlog::logger> logger = nullptr);

        /// Populates the workplaces in geogrid with persons
        void Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig) override;

private:
        /// Assign a workplace to an active person
        void AssignActive(stride::Person* person);

        /// Calculates the workplaces to which persons at this Location may commute to.
        void CommutingLocations(const std::shared_ptr<Location>& loc, double fractionCommuteStudents);

        /// Calculates the fraction of student population that commutes.
        double FractionCommutingStudents();

        /// Calculates the workplaces which are nearby Location loc.
        void NearbyWorkspacePools(GeoGrid& geoGrid, std::shared_ptr<Location> loc);

        /// Determines workplace pools at each Wocation.
        // void WorkplacePoolsAtLocation(GeoGrid& geoGrid);

private:
        GeoGridConfig                     m_geogrid_config;  ///< The GeoGridConfig used during populating.
        std::vector<stride::ContactPool*> m_nearby_wp;       ///< Workplaces near current location.
        std::function<int()>              m_gen_non_commute; ///< Genrator to select nerby workplace.

        std::vector<Location*> m_commuting_locations; ///< Workplaces one may commute to from current loaction.
        std::function<int()>   m_gen_commute;         ///< Generator to select commuting workplace.
};

} // namespace geopop
