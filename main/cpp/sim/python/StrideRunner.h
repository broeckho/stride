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
 * Header for the StrideRunner class.
 */

#include "sim/python/SimulatorObserver.h"
#include "util/Stopwatch.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <string>
#include <vector>

namespace stride {

class Sim;

namespace python {

/**
 * Class for setting up & managing a simulation.
 */
class StrideRunner
{
public:
        /// Constructor
        StrideRunner();

        /// Destructor
        virtual ~StrideRunner() = default;

        /// Register observer (method used by the python environment).
        void RegisterObserver(std::shared_ptr<python::SimulatorObserver>& observer);

        /// Actually setup de run of the simulator
        /// \param track_index_case     whether only tracking index case or doing a full simulation.
        /// \param config_file_name     name of the configuration file for this run
        /// \param use_install_dirs     where to use the files in the install directory
        void Setup(bool track_index_case, const std::string& config_file_name, bool use_install_dirs = false);

        /// Run the simulator with config information provided.
        void Run();

        /// Stop the run.
        void Stop();

        /// Get the simulator (method used by the python environment).
        std::shared_ptr<Sim> GetSimulator() { return m_sim; }

private:
        bool                        m_is_running;    ///< Sim is running.
        std::string                 m_output_prefix; ///< Prefix for outpu data files.
        boost::property_tree::ptree m_pt_config;     ///< Ptree with configuration.
        util::Stopwatch<>           m_clock;         ///< Stopwatch for timing the computation.
        std::shared_ptr<Sim>        m_sim;           ///< Simulator object.
};

} // namespace python
} // namespace stride
