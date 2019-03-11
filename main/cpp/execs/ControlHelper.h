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
 * Header for the command line controller.
 */

#pragma once

#include "util/RnMan.h"
#include "util/Stopwatch.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace stride {

class SimRunner;

/**
 * Controls a simulation run initiated with the command line interface (cli).
 *
 * ControlHelper functions effected on behalf of the controller include:
 * \li checks the OpenMP environment
 * \li checks the file system environment
 * \li interprets and executes the ouput prefix
 * \li install a stride logger)
 * \li a utility method to register the appropriate viewers
 */
class ControlHelper
{
public:
        /// Straight initialization.
        explicit ControlHelper(std::string name, const boost::property_tree::ptree& config);

protected:
        /// Empty controller: used as target for delegation.
        explicit ControlHelper();

        /// Check install environment.
        void CheckEnv();

        // Output_prefix: if it's a string not containing any / it gets interpreted as a
        // filename prefix; otherwise we 'll create the corresponding directory.
        void CheckOutputPrefix();

        /// Make the appropriate logger for cli environment and register as stride_logger.
        void InstallLogger();

        /// Logs info on setup for cli environment to stride_logger.
        void LogShutdown();

        /// Logs info on setup for cli environment to stride_logger.
        void LogStartup();

        /// Register the viewers of the SimRunner.
        void RegisterViewers(std::shared_ptr<SimRunner> runner);

protected:
        boost::property_tree::ptree     m_config;           ///< Main configuration for run and sim.
        std::string                     m_name;             ///< Contoller's name.
        std::string                     m_output_prefix;    ///< Prefix to output (name prefix or prefix dir)
        util::RnMan                     m_rn_man;           ///< Manager for random numbers
        util::Stopwatch<>               m_run_clock;        ///< Stopwatch for timing the computation.
        std::shared_ptr<spdlog::logger> m_stride_logger;    ///< General logger.
        bool                            m_use_install_dirs; ///< Working dir or install dir mode.
};

} // namespace stride
