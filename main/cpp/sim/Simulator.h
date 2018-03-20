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

#include "core/ContactLogMode.h"
#include "core/ContactPool.h"
#include "core/ContactProfiles.h"
#include "core/DiseaseProfile.h"
#include "sim/python/SimulatorObserver.h"
#include "sim/python/Subject.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>

namespace spdlog {
class logger;
}

namespace stride {

class Calendar;
class Population;

/**
 * Simulator can time step and reveal some of the key data.
 * The Subject base class used for the interaction with the python environment only.
 */
class Simulator : public python::Subject<unsigned int, python::SimulatorObserver>
{
public:
        /// Default constructor for empty Simulator.
        Simulator();

        /// Check if the simulator is operational.
        bool IsOperational() const { return m_operational; }

        /// Caledar associated with simulated world. Represents date/simulated day of
        /// last TimeStep completed (it is incremented at the very end of TimeStep).
        std::shared_ptr<Calendar> GetCalendar() const { return m_calendar; }

        /// Get the disease profile.
        const DiseaseProfile& GetDiseaseProfile() const { return m_disease_profile; }

        /// Get the population.
        std::shared_ptr<Population> GetPopulation() { return m_population; }

        /// Run one time step, computing full simulation (default) or only index case.
        void TimeStep();

private:
        /// Update the contacts in the given contactpools.
        template <ContactLogMode::Id log_level, typename local_information_policy, bool track_index_case = false>
        void UpdateContactPools();

private:
        boost::property_tree::ptree     m_config_pt;        ///< Configuration property tree
        ContactLogMode::Id              m_contact_log_mode; ///< Specifies contact/transmission logging mode.
        std::shared_ptr<spdlog::logger> m_contact_logger;   ///< Logger for contact/transmission.
        ContactProfiles                 m_contact_profiles; ///< Contact patterns.
        DiseaseProfile                  m_disease_profile;  ///< Profile of disease.
        unsigned int                    m_num_threads;      ///< The number of (OpenMP) threads.
        bool                            m_track_index_case; ///< General simulation or tracking index case.

        std::shared_ptr<Calendar> m_calendar;    ///< Management of calendar.
        bool                      m_operational; ///< False when invalid disease profile is specified.
        util::RNManager           m_rn_manager;  ///< Random numbere generation management.

private:
        ///< Last simulated day; in TimeStep it is the currently simulating day i.e. m_sim_day is incremented at the
        ///< beginning of TimeStep and should be used with coution inside TimeStep.
        unsigned int m_sim_day;

private:
        std::shared_ptr<Population> m_population;               ///< Pointer to the Population.
        std::vector<ContactPool>    m_households;               ///< Container with household ContactPools.
        std::vector<ContactPool>    m_school_pools;             ///< Container with school ContactPools.
        std::vector<ContactPool>    m_work_pools;               ///< Container with work ContactPools.
        std::vector<ContactPool>    m_primary_community;        ///< Container with primary community ContactPools.
        std::vector<ContactPool>    m_secondary_community;      ///< Container with secondary community ContactPools.
        std::string                 m_local_information_policy; ///< Local information name.

private:
        friend class SimulatorBuilder;
        friend class Vaccinator;
};

} // namespace stride
