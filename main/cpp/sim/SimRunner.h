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
 * Header for the SimRunner class.
 */

#pragma once

#include "sim/event/Id.h"
#include "sim/event/Subject.h"
#include "util/RnMan.h"
#include "util/Stopwatch.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <string>

namespace stride {

class Sim;
class Population;

/**
 * The simulation runner drive simulator throufgh time steps.
 * It's functions are:
 * \li invokes the simulator builder (@see SimulatorBuilder)
 * \li manages elapsed time clock
 * \li manages time steps
 * \linotifies viewers of its events (@see sim_event::Id)
 */
class SimRunner : public util::Subject<stride::sim_event::Id>
{
public:
        /// Initialization with property tree.
        /// \param configPt config info for run and for config of simulator
        explicit SimRunner(const boost::property_tree::ptree& configPt, std::shared_ptr<Population> pop,
                           util::RnMan& rnManager);

        /// Destructor
        ~SimRunner() override = default;

        /// Return the run & sim configuration.
        const util::Stopwatch<>& GetClock() const { return m_clock; }

        /// Return the run & sim configuration.
        const boost::property_tree::ptree& GetConfig() const { return m_config; }

        /// Return the Simulator.
        std::shared_ptr<Sim> GetSim() const { return m_sim; }

        /// Run simulator for as many steps/days as indicated in config.
        void Run();

        /// Run simulator for numSteps steps/days.
        void Run(unsigned int numSteps);

private:
        util::Stopwatch<>           m_clock;         ///< Stopwatch for timing the computation.
        boost::property_tree::ptree m_config;        ///< Ptree with configuration.
        std::string                 m_output_prefix; ///< Prefix for output data files.
        std::shared_ptr<Sim>        m_sim;           ///< Simulator object.
};

} // namespace stride
