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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the Simulator class.
 */

#include "contact/AgeContactProfiles.h"
#include "contact/ContactLogMode.h"
#include "contact/Infector.h"
#include "contact/TransmissionProfile.h"
#include "sim/python/SimulatorObserver.h"
#include "sim/python/Subject.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <map>
#include <tuple>

namespace stride {

class Calendar;
class Population;

/**
 * Simulator can time step and reveal some of the key data.
 * The Subject base class used for the interaction with the python environment only.
 */
class Sim : public python::Subject<unsigned int, python::SimulatorObserver>
{
public:
        /// Default constructor for empty Simulator.
        Sim();

        /// Calendar associated with simulated world. Gets initialized with the date
        /// in the simulation world at whic simulation starts. It represents date/simulated day
        /// of the last TimeStep completed (it is incremented at the very end of TimeStep).
        /// GetCalendar()->GetsimulationDay() is the number of days simulated in the alst
        /// completed time step.
        std::shared_ptr<Calendar> GetCalendar() const { return m_calendar; }

        /// Get the transmission profile.
        const TransmissionProfile& GetTransmissionProfile() const { return m_transmission_profile; }

        /// Get the population.
        std::shared_ptr<Population> GetPopulation() { return m_population; }

        /// Get the random number manager.
        util::RNManager& GetRNManager() { return m_rn_manager; }

        /// Run one time step, computing full simulation (default) or only index case.
        void TimeStep();

private:
        /// Update the contacts in the given contactpools.
        void UpdatePools();

        using InfectorMap = std::map<std::tuple<stride::ContactLogMode::Id, bool, std::string>, InfectorExecT>;
        InfectorMap m_infectors;

private:
        boost::property_tree::ptree m_config_pt;            ///< Configuration property tree
        ContactLogMode::Id          m_contact_log_mode;     ///< Specifies contact/transmission logging mode.
        AgeContactProfiles          m_contact_profiles;     ///< Contact profiles w.r.t age.
        unsigned int                m_num_threads;          ///< The number of (OpenMP) threads.
        bool                        m_track_index_case;     ///< General simulation or tracking index case.
        TransmissionProfile         m_transmission_profile; ///< Profile of disease.
        std::string                 m_local_info_policy;    ///< Local information policy name.

        std::shared_ptr<Calendar>   m_calendar;   ///< Managment of calendar.
        std::shared_ptr<Population> m_population; ///< Pointer to the Population.
        util::RNManager             m_rn_manager; ///< Random numbere generation management.

private:
        friend class SimBuilder;
};

} // namespace stride
