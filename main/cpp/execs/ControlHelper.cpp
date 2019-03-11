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

#include "ControlHelper.h"

#include "pop/Population.h"
#include "sim/SimRunner.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/TimeStamp.h"
#include "viewers/CliViewer.h"
#include "viewers/InfectedFileViewer.h"
#include "viewers/PersonsFileViewer.h"
#include "viewers/SummaryFileViewer.h"

#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace stride::util;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {

ControlHelper::ControlHelper()
    : m_config(), m_name(), m_output_prefix(), m_rn_man(), m_run_clock("run"), m_stride_logger(nullptr),
      m_use_install_dirs()

{
}

ControlHelper::ControlHelper(string name, const ptree& config) : ControlHelper()
{
        m_run_clock.Start();
        m_config           = config;
        m_name             = std::move(name);
        m_output_prefix    = m_config.get<string>("run.output_prefix");
        m_use_install_dirs = m_config.get<bool>("run.use_install_dirs");

        m_rn_man.Initialize(RnMan::Info{m_config.get<string>("pop.rng_seed", "1,2,3,4"), "",
                                        m_config.get<unsigned int>("run.num_threads")});
}

void ControlHelper::CheckEnv()
{
        if (m_use_install_dirs) {
                auto log = [](const string& s) -> void { cerr << s << endl; };
                if (!FileSys::CheckInstallEnv(log)) {
                        throw runtime_error("ControlHelper::CheckEnv> Install dirs not OK.");
                }
        }
}

void ControlHelper::CheckOutputPrefix()
{
        if (FileSys::IsDirectoryString(m_output_prefix)) {
                FileSys::CreateDirectory(m_output_prefix);
        }
}

void ControlHelper::InstallLogger()
{
        // spd log levels are: trace, debug, info, warn, error, critical, off
        const auto path     = FileSys::BuildPath(m_output_prefix, "stride_log.txt");
        const auto logLevel = m_config.get<string>("run.stride_log_level");
        m_stride_logger     = LogUtils::CreateCliLogger("stride_logger", path.string());
        m_stride_logger->set_level(spdlog::level::from_str(logLevel));
        m_stride_logger->flush_on(spdlog::level::err);
        spdlog::register_logger(m_stride_logger);
}

void ControlHelper::LogShutdown()
{
        m_run_clock.Stop();
        m_stride_logger->info("{} shutting down after: {}", m_name, m_run_clock.ToString());
}

void ControlHelper::LogStartup()
{
        m_stride_logger->info("{} starting up at: {}", m_name, TimeStamp().ToString());
        m_stride_logger->info("Executing revision: {}", ConfigInfo::GitRevision());
        m_stride_logger->info("Processor count: {}", ConfigInfo::ProcessorCount());
        m_stride_logger->info("Creating dir:  {}", m_output_prefix);
        m_stride_logger->trace("Executing:           {}", FileSys::GetExecPath().string());
        m_stride_logger->trace("Current directory:   {}", FileSys::GetCurrentDir().string());
        if (m_use_install_dirs) {
                m_stride_logger->trace("Install directory:   {}", FileSys::GetRootDir().string());
                m_stride_logger->trace("Config  directory:   {}", FileSys::GetConfigDir().string());
                m_stride_logger->trace("Data    directory:   {}", FileSys::GetDataDir().string());
        }
        if (ConfigInfo::HaveOpenMP()) {
                m_stride_logger->info("Max number OpenMP threads in this environment: {}",
                                      ConfigInfo::NumberAvailableThreads());
                m_stride_logger->info("Configured number of threads: {}",
                                      m_config.get<unsigned int>("run.num_threads"));
        } else {
                m_stride_logger->info("Not using OpenMP threads.");
        }
}

void ControlHelper::RegisterViewers(shared_ptr<SimRunner> runner)
{
        // Command line viewer
        m_stride_logger->info("Registering CliViewer");
        const auto cli_v = make_shared<viewers::CliViewer>(runner, m_stride_logger);
        runner->Register(cli_v, bind(&viewers::CliViewer::Update, cli_v, placeholders::_1));

        // Infection counts viewer
        if (m_config.get<bool>("run.output_cases", false)) {
                m_stride_logger->info("Registering InfectedFileViewer");
                const auto v = make_shared<viewers::InfectedFileViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::InfectedFileViewer::Update, v, placeholders::_1));
        }

        // Persons viewer
        if (m_config.get<bool>("run.output_persons", false)) {
                m_stride_logger->info("Registering PersonsFileViewer.");
                const auto v = make_shared<viewers::PersonsFileViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::PersonsFileViewer::Update, v, placeholders::_1));
        }

        // Summary viewer
        if (m_config.get<bool>("run.output_summary", false)) {
                m_stride_logger->info("Registering SummaryFileViewer");
                const auto v = make_shared<viewers::SummaryFileViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::SummaryFileViewer::Update, v, placeholders::_1));
        }
}

} // namespace stride
