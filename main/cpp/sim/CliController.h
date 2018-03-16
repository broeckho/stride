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

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <spdlog/spdlog.h>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

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
              m_output_prefix(""),
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

        // Output_prefix: if it's a string not containing any / it gets interpreted as a
        // filename prefix; otherwise we 'll create the corresponding directory.
        void CheckOutputPrefix();

        /// Make the appropriate logger for cli environment and register as stride_logger.
        void MakeLogger();

        /// Patch run configuration with cli overrides and defaults.
        void PatchConfig();

        /// Read configuration file.
        void ReadConfigFile();

        /// Register the viewers of the SimRunner.
        void RegisterViewers(std::shared_ptr<SimRunner> runner);

private:
        std::string                                       m_config_file;
        bool                                              m_track_index_case;
        unsigned int                                      m_max_num_threads;
        std::string m_output_prefix;
        std::vector<std::tuple<std::string, std::string>> m_p_overrides;
        bool                                              m_silent_mode;
        bool                                              m_use_install_dirs;

        util::Stopwatch<>               m_run_clock; ///< Stopwatch for timing the computation.
        std::shared_ptr<spdlog::logger> m_logger;    ///< General logger.
        boost::filesystem::path   m_config_path; ///< path to config file.
        boost::property_tree::ptree     m_config_pt; ///< Main configuration for run and sim.
};

} // namespace stride