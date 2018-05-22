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
#include "contact/ContactHandler.h"
#include "contact/ContactLogMode.h"
#include "contact/InfectorExec.h"
#include "contact/TransmissionProfile.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace stride {

class Calendar;
class Population;

/**
 * Simulator can time step and reveal some of the key data.
 * The Subject base class used for the interaction with the python environment only.
 */
class Sim
{
public:
        /// Create Sim initialized by the configuration in property tree and population.
        static std::shared_ptr<Sim> Create(const boost::property_tree::ptree& configPt,
                                           std::shared_ptr<Population>        pop);

        /// For use in python environment: create using configuration string i.o ptree.
        static std::shared_ptr<Sim> Create(const std::string& configString, std::shared_ptr<Population> pop);

        /// Calendar for the simulated world. Initialized with the start date in the simulation
        /// world. Use GetCalendar()->GetSimulationDay() for the number of days simulated.
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
        /// Default constructor for empty Simulator.
        Sim();

        /// SimBuilder accesses the default constructor to build Sim using config.
        friend class SimBuilder;

private:
        boost::property_tree::ptree m_config_pt;         ///< Configuration property tree
        ContactLogMode::Id          m_contact_log_mode;  ///< Specifies contact/transmission logging mode.
        unsigned int                m_num_threads;       ///< The number of (OpenMP) threads.
        bool                        m_track_index_case;  ///< General simulation or tracking index case.
        std::string                 m_local_info_policy; ///< Local information policy name.

        std::shared_ptr<Calendar>   m_calendar;             ///< Managment of calendar.
        AgeContactProfiles          m_contact_profiles;     ///< Contact profiles w.r.t age.
        std::vector<ContactHandler> m_handlers;             ///< Contact handlers (rng & rates).
        InfectorExec*               m_infector;             ///< Executes contacts/transmission loops in contact pool.
        std::shared_ptr<Population> m_population;           ///< Pointer to the Population.
        util::RNManager             m_rn_manager;           ///< Random numbere generation management.
        TransmissionProfile         m_transmission_profile; ///< Profile of disease.
};

} // namespace stride
