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
 * Header for the commnad line controller.
 */

#include "util/Stopwatch.h"

#include <boost/property_tree/ptree.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <tuple>
#include <utility>

namespace stride {

class SimRunner;

class CliController
{
public:
        /// Straight initialization.
        CliController(bool track_index_case, std::string config_file,
                      std::vector<std::tuple<std::string, std::string>> p_overrides, bool silent_mode = false,
                      bool use_install_dirs = true)
            : m_config_file(std::move(config_file)), m_track_index_case(track_index_case), m_max_num_threads(1U),
              m_p_overrides(std::move(p_overrides)), m_silent_mode(silent_mode), m_use_install_dirs(use_install_dirs),
              m_run_clock("run_clock", true){};

        /// Actual run of the simulator.
        void Go();

        /// Setup the controller.
        void Setup();

private:
        /// Check install environment.
        void CheckEnv();

        /// Check the OpenMP environment.
        void CheckOpenMP();

        /// Register the viewers of the SimRunner.
        void RegisterViewers(std::shared_ptr<SimRunner> runner, const std::string& output_prefix);

        /// Setup and patch run configuration file.
        void SetupConfig();

        /// Acquire logger for normal mode.
        std::shared_ptr<spdlog::logger> SetupLogger();

        /// Acquire null logger for silent mode.
        std::shared_ptr<spdlog::logger> SetupNullLogger();

private:
        std::string                                       m_config_file;
        bool                                              m_track_index_case;
        unsigned int                                      m_max_num_threads;
        std::vector<std::tuple<std::string, std::string>> m_p_overrides;
        bool                                              m_silent_mode;
        bool                                              m_use_install_dirs;

        util::Stopwatch<>               m_run_clock; ///< Stopwatch for timing the computation.
        std::shared_ptr<spdlog::logger> m_logger;    ///< General logger.
        boost::property_tree::ptree     m_config_pt; ///< Main configuration for run and sim.
};

} // namespace stride