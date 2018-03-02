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

#include <boost/property_tree/ptree.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>

namespace stride {

class CliController
{
public:
        /// Straight initialization.
        CliController(bool track_index_case, std::string config_file, bool silent_mode = false,
                      bool use_install_dirs = false)
            : m_track_index_case(track_index_case), m_config_file(std::move(config_file)),
              m_silent_mode(silent_mode), m_use_install_dirs(use_install_dirs) {};

        /// Actually setup the run of the simulator.
        bool Go();

        /// Acquire logger for normal mode.
        bool SetupLogger();

        /// Acquire null logger for silent mode.
        bool SetupNullLogger();

private:
        /// Check and patch (if necessary) main configuration file.
        bool CheckConfig();

        /// Check install environment.
        bool CheckEnv();

        /// Check the OpenMP environment.
        bool CheckOpenMP();

private:
        bool        m_track_index_case;
        std::string m_config_file;
        bool        m_silent_mode;
        bool        m_use_install_dirs;

        std::shared_ptr<spdlog::logger> m_logger;
        boost::property_tree::ptree     m_config_pt;
};

} // namespace stride