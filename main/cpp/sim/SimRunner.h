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
 * Header for the SimRunner class.
 */

#include "sim/event/Id.h"
#include "sim/event/Subject.h"
#include "sim/python/SimulatorObserver.h"
#include "util/Stopwatch.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <string>
#include <vector>

namespace spdlog {
class logger;
}

namespace stride {

class Sim;

/**
 * The simulation runner build a simulator and then lets it step through
 * untill the end of the simulation interval.
 * The SimRunner setup
 * \li accepts and configuration property tree from its controller
 * \li makes a logger
 * \li outputs the run configuration to file
 * \li invokes the builder (@see SimulatorBuilder) for the simulator
 * \li checks the simulator
 * The SimRunner execution
 * \li manages elapsed time clock
 * \li manages time steps
 * All the while SimRunner notifies viewers of its events (@see sim_event::Id)
 */
class SimRunner : public util::Subject<stride::sim_event::Id>
{
public:
        /// Default initialization.
        SimRunner();

        /// Destructor
        virtual ~SimRunner() = default;

        /// Return the run & sim configuration.
        const util::Stopwatch<>& GetClock() const { return m_clock; }

        /// Return the run & sim configuration.
        const boost::property_tree::ptree& GetConfig() const { return m_config_pt; }

        /// Return the Simulator.
        std::shared_ptr<Sim> GetSim() const { return m_sim; }

        /// Setup the context for the simulation run.
        /// \param config_pt        config info for run and for config of simulator
        /// \param logger               general logger
        void Setup(const boost::property_tree::ptree& config_pt, std::shared_ptr<spdlog::logger> logger = nullptr);

        /// Run simulator for numDays steps.
        void Run();

public:
        ///
        void AtFinish();

        ///
        void AtStart();

        /// Run simulator for numDays steps.
        void Run(unsigned int numDays);

private:
        /// Private constructor, @see Create.


private:
        util::Stopwatch<>               m_clock;         ///< Stopwatch for timing the computation.
        std::string                     m_output_prefix; ///< Prefix for output data files.
        boost::property_tree::ptree     m_config_pt;     ///< Ptree with configuration.
        std::shared_ptr<Sim>            m_sim;           ///< Simulator object.
        std::shared_ptr<spdlog::logger> m_stride_logger; ///< General logger (!= contact_logger).
};

} // namespace stride
