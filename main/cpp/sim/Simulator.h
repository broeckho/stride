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
 * Header for the Simulator class.
 */

#include "calendar/Calendar.h"
#include "core/Cluster.h"
#include "core/ContactProfiles.h"
#include "core/DiseaseProfile.h"
#include "core/LogMode.h"
#include "core/RngHandler.h"
#include "pop/Population.h"
#include "sim/SimulatorObserver.h"
#include "util/Subject.h"

#include <boost/property_tree/ptree.hpp>
#include <array>

namespace stride {

/**
 * Main class that contains and direct the virtual world.
 */
class Simulator : public util::Subject<unsigned int, SimulatorObserver>
{
public:
        /// Default constructor for empty Simulator.
        Simulator();

        /// Get the population.
        std::shared_ptr<Population> GetPopulation() { return m_population; }

        /// Get the disease profile.
        const DiseaseProfile GetDiseaseProfile() const { return m_disease_profile; }

        /// Check if the simulator is operational.
        bool IsOperational() const { return GetDiseaseProfile().IsOperational(); }

        /// Change track_index_case setting.
        void SetTrackIndexCase(bool track_index_case) { m_track_index_case = track_index_case; }

        /// Run one time step, computing full simulation (default) or only index case.
        void TimeStep();

private:
        /// Update the contacts in the given clusters.
        template <LogMode::Id log_level, typename local_information_policy, bool track_index_case = false>
        void UpdateClusters();

private:
        boost::property_tree::ptree m_pt_config; ///< Configuration property tree

private:
        std::string m_local_information_policy; ///<

private:
        unsigned int m_num_threads;            ///< The number of (OpenMP) threads.
        std::vector<RngHandler> m_rng_handler; ///< Pointer to the RngHandlers.
        LogMode::Id m_log_level;               ///< Specifies logging mode.
        std::shared_ptr<Calendar> m_calendar;  ///< Management of calendar.

private:
        std::shared_ptr<Population> m_population; ///< Pointer to the Population.

        std::vector<Cluster> m_households;          ///< Container with household Clusters.
        std::vector<Cluster> m_school_clusters;     ///< Container with school Clusters.
        std::vector<Cluster> m_work_clusters;       ///< Container with work Clusters.
        std::vector<Cluster> m_primary_community;   ///< Container with primary community Clusters.
        std::vector<Cluster> m_secondary_community; ///< Container with secondary community  Clusters.

        ContactProfiles m_contact_profiles; ///< Contact patterns.
        DiseaseProfile m_disease_profile;   ///< Profile of disease.

        bool m_track_index_case; ///< General simulation or tracking index case.

private:
        friend class SimulatorBuilder;
        friend class Vaccinator;
};

} // namespace stride
