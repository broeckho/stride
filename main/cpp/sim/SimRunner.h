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

#include "event/Subject.h"
#include "sim/Simulator.h"
#include "sim/event/Payload.h"
#include "sim/python/SimulatorObserver.h"
#include "util/Stopwatch.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace stride {

/**
 * Managing a run of the simulator.
 */
class SimRunner : public util::Subject<stride::sim_event::Payload>
{
public:
        /// Constructor
        SimRunner();

        /// Destructor
        virtual ~SimRunner() = default;

        /// Setup the context for the simulation run.
        /// \param run_config_pt        config info for run and for config of simulator
        /// \param logger               generel logger
        /// \return                     status value
        bool Setup(const boost::property_tree::ptree& run_config_pt, std::shared_ptr<spdlog::logger> logger);

        /// Run the simulator with config information provided.
        void Run();

private:
        bool                            m_is_running;    ///< Sim is running.
        bool                            m_operational;   ///< Input config is OK to be run
        std::string                     m_output_prefix; ///< Prefix for outpu data files.
        boost::property_tree::ptree     m_pt_config;     ///< Ptree with configuration.
        std::shared_ptr<Simulator>      m_sim;           ///< Simulator object.
        std::shared_ptr<spdlog::logger> m_logger;        ///< General logger.
};

} // namespace stride
