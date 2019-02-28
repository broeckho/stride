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

#pragma once

#include "contact/AgeContactProfiles.h"
#include "contact/ContactHandler.h"
#include "contact/ContactLogMode.h"
#include "contact/InfectorExec.h"
#include "contact/TransmissionProfile.h"
#include "disease/PublicHealthAgency.h"

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace stride {

class Calendar;
class Population;

namespace util {
class RnMan;
}

/**
 * Simulator can time step and reveal some of the key data.
 * The Subject base class used for the interaction with the python environment only.
 */
class Sim
{
public:
        /// Create Sim initialized by the configuration in property tree and population.
        static std::shared_ptr<Sim> Create(const boost::property_tree::ptree& configPt, std::shared_ptr<Population> pop,
                                           util::RnMan& rnManager);

        /// For use in SWIG: use shared_ptr to an rnManager and make the Sim have ownership so it won't be destroyed.
        /// It cannot be owned by the python environment since SWIG cannot handle the RnMan.
        static std::shared_ptr<Sim> Create(const boost::property_tree::ptree& configPt, std::shared_ptr<Population> pop,
                                           std::shared_ptr<util::RnMan> rnManager);

        /// Calendar for the simulated world. Initialized with the start date in the simulation
        /// world. Use GetCalendar()->GetSimulationDay() for the number of days simulated.
        std::shared_ptr<Calendar> GetCalendar() const { return m_calendar; }

        /// Get the transmission profile.
        const TransmissionProfile& GetTransmissionProfile() const { return m_transmission_profile; }

        /// Get the population.
        std::shared_ptr<Population> GetPopulation() { return m_population; }

        /// Get the random number manager.
        util::RnMan& GetRnManager() { return m_rn_manager; }

        /// Run one time step, computing full simulation (default) or only index case.
        void TimeStep();

        /// Get the Sim configuration for the given attribute.
        std::string GetConfigValue(const std::string& attribute) const
        {
                return m_config.get<std::string>(attribute);
        }

        /// Get the stored transmission rate.
        double GetTransmissionRate() const { return m_transmission_profile.GetRate(); }

private:
        /// Constructor for empty Simulator.
        explicit Sim(util::RnMan&);

        /// Constructor for empty Simulator, used in Python environment
        explicit Sim(std::shared_ptr<util::RnMan> rnMan);

        /// SimBuilder accesses the default constructor to build Sim using config.
        friend class SimBuilder;

private:
        boost::property_tree::ptree m_config;                        ///< Configuration property tree
        ContactLogMode::Id          m_contact_log_mode;              ///< Specifies contact/transmission logging mode.
        unsigned int                m_num_threads;                   ///< The number of (OpenMP) threads.
        bool                        m_track_index_case;              ///< General simulation or tracking index case.
        bool                        m_adaptive_symptomatic_behavior; ///< Should symptomatic cases stay home?

        std::shared_ptr<Calendar>   m_calendar;         ///< Management of calendar.
        AgeContactProfiles          m_contact_profiles; ///< Contact profiles w.r.t age.
        std::vector<ContactHandler> m_handlers;         ///< Contact handlers (rng & rates).
        InfectorExec*               m_infector;         ///< Executes contacts/transmission loops in contact pool.
        std::shared_ptr<Population> m_population;       ///< Pointer to the Population.
        util::RnMan&                m_rn_manager;       ///< Random number generation management.

        TransmissionProfile m_transmission_profile; ///< Profile of disease.
        PublicHealthAgency  m_public_health_agency; ///< Agency to implement reactive strategies.

private:
        ///< Used when created from the Python environment to prevent it from being destructed.
        std::shared_ptr<util::RnMan> m_rn_manager_ptr = nullptr;
};

} // namespace stride
