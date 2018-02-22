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
#include "core/ContactPool.h"
#include "core/ContactProfiles.h"
#include "core/DiseaseProfile.h"
#include "core/LogMode.h"
#include "pop/Population.h"
#include "sim/python/SimulatorObserver.h"
#include "sim/python/Subject.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <array>

namespace stride {

/**
 * Main class that contains and direct the virtual world.
 */
class Simulator : public python::Subject<unsigned int, python::SimulatorObserver>
{
public:
        /// Default constructor for empty Simulator.
        Simulator();

        /// Get the population.
        std::shared_ptr<Population> GetPopulation() { return m_population; }

        /// Get the disease profile.
        const DiseaseProfile GetDiseaseProfile() const { return m_disease_profile; }

        /// Check if the simulator is operational.
        bool IsOperational() const { return m_operational; }

        /// Run one time step, computing full simulation (default) or only index case.
        void TimeStep();

private:
        /// Update the contacts in the given clusters.
        template <LogMode::Id log_level, typename local_information_policy, bool track_index_case = false>
        void UpdateContactPools();

private:
        boost::property_tree::ptree m_pt_config; ///< Configuration property tree

private:
        std::string m_local_information_policy; ///<

private:
        unsigned int              m_num_threads; ///< The number of (OpenMP) threads.
        LogMode::Id               m_log_level;   ///< Specifies logging mode.
        std::shared_ptr<Calendar> m_calendar;    ///< Management of calendar.
        util::RNManager           m_rn_manager;  ///< Random numbere generation management.
private:
        std::shared_ptr<Population> m_population; ///< Pointer to the Population.

        std::vector<ContactPool> m_households;          ///< Container with household ContactPools.
        std::vector<ContactPool> m_school_pools;        ///< Container with school ContactPools.
        std::vector<ContactPool> m_work_pools;          ///< Container with work ContactPools.
        std::vector<ContactPool> m_primary_community;   ///< Container with primary community ContactPools.
        std::vector<ContactPool> m_secondary_community; ///< Container with secondary community ContactPools.

        ContactProfiles m_contact_profiles; ///< Contact patterns.
        DiseaseProfile  m_disease_profile;  ///< Profile of disease.
        bool            m_operational;      ///< Gets to be false when invalid disease profile is specified.

        bool m_track_index_case; ///< General simulation or tracking index case.

private:
        friend class SimulatorBuilder;
        friend class Vaccinator;
};

} // namespace stride
