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

#include <boost/property_tree/xml_parser.hpp>
#include <memory>
#include <vector>

using namespace std;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {

bool CliController::Go()
{
        // Intro
        bool status = true;
        status      = SetupLogger();
        m_logger->info("\n*************************************************************");
        m_logger->info("Starting up at:      {}", TimeStamp().ToString());

        // Preliminary checks.
        if (m_use_install_dirs) {
                status = CheckEnv();
        }
        status = CheckConfig() && CheckOpenMP();

        // Run iff everything OK
        if (status) {
                // Setup simultaion run
                SimRunner runner;
                runner.Setup(m_config_pt, m_logger, m_use_install_dirs);

                // Register viewers
                auto                                           cli = std::make_shared<viewers::CliViewer>(true);
                std::function<void(const sim_event::Payload&)> f =
                    std::bind(&viewers::CliViewer::update, cli, std::placeholders::_1);
                runner.stride::util::Subject<stride::sim_event::Payload>::Register(cli, f);

                // Execute run
                runner.Run();
        }

        // Clean up
        spdlog::drop_all();

        return status;
}

bool CliController::SetupLogger()
{
        bool status = true;
        spdlog::set_async_mode(1048576);
        try {
                std::vector<spdlog::sink_ptr> sinks;
                // auto stdout_sink = spdlog::sinks::stdout_sink_st::instance();
                auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
                sinks.push_back(color_sink);
                const string fn = string("stride_log_").append(TimeStamp().ToTag()).append(".txt");
                sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_st>(fn.c_str()));
                m_logger = std::make_shared<spdlog::logger>("stride_logger", begin(sinks), end(sinks));
                spdlog::register_logger(m_logger);
        } catch (const spdlog::spdlog_ex& e) {
                std::cerr << "Log initialization failed: " << e.what() << std::endl;
                status = false;
        }
        return status;
}

bool CliController::CheckConfig()
{
        bool       status    = true;
        const auto file_path = canonical(system_complete(m_config_file));

        if (!is_regular_file(file_path)) {
                string s = string("Config file ") + file_path.string() + " not present! Aborting.";
                m_logger->critical("Config file ", file_path.string(), " not present! Aborting.");
                status = false;
        } else {
                m_logger->info("Configuration file:  {}", file_path.string());
                try {
                        read_xml(file_path.string(), m_config_pt);
                } catch (xml_parser_error& e) {
                        m_logger->critical("Error reading", file_path.string(), "\n", e.what());
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
                m_config_pt.put("track_index_case", m_track_index_case);
                m_logger->info("Setting for track_index_case:  {}", m_track_index_case);
        }
        return status;
}

bool CliController::CheckEnv()
{
        bool        status = true;
        InstallDirs dirs;
        m_logger->info("Executing:           {}", dirs.GetExecPath().string());
        m_logger->info("Current directory:   {}", dirs.GetCurrentDir().string());
        m_logger->info("Install directory:   {}", dirs.GetRootDir().string());
        m_logger->info("Data    directory:   {}", dirs.GetDataDir().string());

        if (dirs.GetCurrentDir().compare(dirs.GetRootDir()) != 0) {
                m_logger->critical("Current working dir not install root! Aborting.");
                status = false;
        }
        return status;
}

bool CliController::CheckOpenMP()
{
        const auto num_threads = ConfigInfo::NumberAvailableThreads();
        if (ConfigInfo::HaveOpenMP()) {
                m_logger->info("Number OpenMP threads available in this environment: {}", num_threads);
        } else {
                m_logger->info("Not using OpenMP threads.");
        }
        m_config_pt.put("num_threads", num_threads);
        return true;
}

} // namespace stride