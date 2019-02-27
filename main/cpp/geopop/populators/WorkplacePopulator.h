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
#include "util/LogUtils.h"

#include <trng/discrete_dist.hpp>

namespace geopop {

namespace {
using DiscreteDistType = std::function<trng::discrete_dist::result_type()>;
}

/**
 * Populate the workplaces.
 */

class WorkplacePopulator : public Populator
{
public:
        /// Constructor
        explicit WorkplacePopulator(stride::util::RnMan& rn_manager, std::shared_ptr<spdlog::logger> logger =
                                                                         stride::util::LogUtils::CreateNullLogger());

        /// Populates the workplaces in geogrid with persons
        void Apply(std::shared_ptr<GeoGrid> geogrid, const GeoGridConfig& geoGridConfig) override;

private:
        /// Fills the m_workplacesInCity map
        void CalculateWorkplacesInCity();

        /// Calculates m_fractionCommutingStudents
        void CalculateFractionCommutingStudents();

        /// Calculates the workplaces which are nearby to m_currentLoc
        void CalculateNearbyWorkspaces();

        /// Calculates the workplaces to which persons from m_currentLoc may commute to
        void CalculateCommutingLocations();

        /// Assign a workplace to an active person
        void AssignActive(stride::Person* person);

private:
        unsigned int m_assignedTo0          = 0; ///< Number of persons assigned to no workplace.
        unsigned int m_assignedCommuting    = 0; ///< Number of persons assigned to workplace outside home location.
        unsigned int m_assignedNotCommuting = 0; ///< Amount of persons assigned to workplace at the home location.

        std::shared_ptr<Location> m_currentLoc;    ///< The location for which the workers currently are being assigned.
        std::shared_ptr<GeoGrid>  m_geoGrid;       ///< The GeoGrid which will be populated.
        GeoGridConfig             m_geoGridConfig; ///< The GeoGridConfig used during populating.

        std::unordered_map<Location*, std::pair<std::vector<stride::ContactPool*>, DiscreteDistType>>
            m_workplacesInCity; ///< For each location store workplaces and a distribution fot random selection.

        double m_fractionCommutingStudents; ///< Fraction of the commuting people who are a student.

        std::vector<stride::ContactPool*> m_nearByWorkplaces; ///< Workplaces which are nearby to the m_currentLoc.
        DiscreteDistType                  m_distNonCommuting; ///< Distribution to choose from m_nearByWorkPlaces.

        std::vector<Location*> m_commutingLocations; ///< Workplaces which persons from m_currentLoc may commute to.
        DiscreteDistType       m_disCommuting;       ///< Distribution to choose from m_commutingLocations.
};

} // namespace geopop
