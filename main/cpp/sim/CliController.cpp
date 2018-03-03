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

#include "sim/CliController.h"

#include "event/Subject.h"
#include "sim/SimRunner.h"
#include "util/ConfigInfo.h"
#include "util/InstallDirs.h"
#include "util/StringUtils.h"
#include "util/TimeStamp.h"
#include "viewers/CliViewer.h"

#include "spdlog/sinks/null_sink.h"
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {

bool CliController::CheckEnv()
{
        bool        status = true;
        InstallDirs dirs;
        m_logger->info("Executing:           {}", dirs.GetExecPath().string());
        m_logger->info("Current directory:   {}", dirs.GetCurrentDir().string());
        if (m_use_install_dirs) {
                m_logger->info("Install directory:   {}", dirs.GetRootDir().string());
                m_logger->info("Data    directory:   {}", dirs.GetDataDir().string());
                if (dirs.GetCurrentDir().compare(dirs.GetRootDir()) != 0) {
                        m_logger->critical("Current working dir not install root! Aborting.");
                        status = false;
                }
        }
        return status;
}

bool CliController::CheckOpenMP()
{
        m_max_num_threads = ConfigInfo::NumberAvailableThreads();
        if (ConfigInfo::HaveOpenMP()) {
                m_logger->info("Max number OpenMP threads in this environment: {}", m_max_num_threads);
        } else {
                m_logger->info("Not using OpenMP threads.");
        }
        return true;
}

bool CliController::Go()
{
        // Intro
        bool status = m_silent_mode ? SetupNullLogger() : SetupLogger();
        if (status) {
                m_logger->info("\n*************************************************************");
                m_logger->info("Starting up at:      {}", TimeStamp().ToString());

                // Checks && setup, order important and relies on C++'s short circuit evaluation.
                status = CheckEnv() && CheckOpenMP() && SetupConfig();
        }
        if (status) {
                // Setup simulation run
                m_logger->info("\n\nSetting up the simulation runner.");
                SimRunner runner;
                runner.Setup(m_config_pt, m_logger);

                // Register viewers
                auto c_v = make_shared<viewers::CliViewer>(true);
                runner.Register(c_v, bind(&viewers::CliViewer::update, c_v, placeholders::_1));

                // Execute run
                runner.Run();
        }

        // Clean up
        spdlog::drop_all();

        return status;
}

bool CliController::SetupConfig()
{
        bool       status    = true;
        const auto file_path = canonical(system_complete(m_config_file));

        if (!is_regular_file(file_path)) {
                m_logger->critical("Config file {} ot present! Aborting.", file_path.string());
                status = false;
        } else {
                m_logger->info("Configuration file:  {}", file_path.string());
                try {
                        read_xml(file_path.string(), m_config_pt);
                } catch (xml_parser_error& e) {
                        m_logger->critical("Error reading {} \n Exception: {}", file_path.string(), e.what());
                        status = false;
                }
        }

        // -----------------------------------------------------------------------------------------
        // Fix + commandline overrides of configuration .
        // -----------------------------------------------------------------------------------------
        if (status) {
                if (!m_config_pt.get_optional<bool>("run.num_participants_survey")) {
                        m_config_pt.put("run.num_participants_survey", 1);
                }
                m_config_pt.put("run.track_index_case", m_track_index_case);
                m_logger->info("Setting for run.track_index_case:  {}", m_track_index_case);
                m_config_pt.put("run.use_install_dirs", m_use_install_dirs);
                m_logger->info("Setting for run.use_install_dirs:  {}", m_use_install_dirs);
                for (const auto& p : m_p_overrides) {
                        m_config_pt.put("run." + get<0>(p), get<1>(p));
                        m_logger->info("Commanline override for run.{}:  {}", get<0>(p), get<1>(p));
                }
                if (!m_config_pt.get_optional<bool>("run.num_threads")) {
                        m_config_pt.put("run.num_threads", m_max_num_threads);
                        m_logger->info("Defaulting for run.num_threads:  {}", m_max_num_threads);
                }
        }
        write_xml("lalala", m_config_pt);

        return status;
}

bool CliController::SetupLogger()
{
        bool status = true;
        spdlog::set_async_mode(1048576);
        try {
                vector<spdlog::sink_ptr> sinks;
                auto                     color_sink = make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
                sinks.push_back(color_sink);
                const string fn = string("stride_log_").append(TimeStamp().ToTag()).append(".txt");
                sinks.push_back(make_shared<spdlog::sinks::simple_file_sink_st>(fn.c_str()));
                m_logger = make_shared<spdlog::logger>("stride_logger", begin(sinks), end(sinks));
                spdlog::register_logger(m_logger);
        } catch (const spdlog::spdlog_ex& e) {
                cerr << "Stride logger initialization failed: " << e.what() << endl;
                status = false;
        }
        return status;
}

bool CliController::SetupNullLogger()
{
        bool status = true;
        spdlog::set_async_mode(1048576);
        try {
                auto null_sink = make_shared<spdlog::sinks::null_sink_st>();
                m_logger       = make_shared<spdlog::logger>("stride_logger", null_sink);
                spdlog::register_logger(m_logger);
        } catch (const spdlog::spdlog_ex& e) {
                cerr << "Stride null logger initialization failed: " << e.what() << endl;
                status = false;
        }
        return status;
}

} // namespace stride