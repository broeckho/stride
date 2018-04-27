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
 * Header for the command line controller.
 */

#include "util/Stopwatch.h"

//#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace stride {

class SimRunner;

/**
 * The CliController controls execution of a simulation run (@see SimRunner) from the
 * command line interface (cli).
 * The CliController setup
 * \li accepts the commandline arguments
 * \li checks the OpenMP environment
 * \li checks the file system environment
 * \li reads the config file specified on the cli
 * \li effects cli overides of config parameters
 * \li patches the config file for any remaining defaults
 * \li interprets and executes the ouput prefix
 * \li makes a stride logger
 * The CliController execution
 * \li creates a simulation runner (@see SimRunner)
 * \li registers the appropriate viewers
 * \li runs the simulation
 */
class CliController
{
public:
        /// Straight initialization.
        explicit CliController(const boost::property_tree::ptree& configPt);

        /// Actual run of the simulator.
        void Control();

private:
        /// Empty controller: used as taget for delegation.
        explicit CliController();

        /// Check install environment.
        void CheckEnv();

        // Output_prefix: if it's a string not containing any / it gets interpreted as a
        // filename prefix; otherwise we 'll create the corresponding directory.
        void CheckOutputPrefix();

        /// Logs info on setup for cli environment to stride_logger.
        void LogSetup();

        /// Make the appropriate logger for cli environment and register as stride_logger.
        void MakeLogger();

        /// Register the viewers of the SimRunner.
        void RegisterViewers(std::shared_ptr<SimRunner> runner);

private:
        boost::property_tree::ptree     m_config_pt;        ///< Main configuration for run and sim.
        std::string                     m_output_prefix;    /// Prefix to output (name prefix or prefix dir)
        util::Stopwatch<>               m_run_clock;        ///< Stopwatch for timing the computation.
        std::shared_ptr<spdlog::logger> m_stride_logger;    ///< General logger.
        bool                            m_use_install_dirs; /// Working dir or install dir mode.
};

} // namespace stride
