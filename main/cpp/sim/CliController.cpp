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

#include "sim/CliController.h"

#include "sim/SimRunner.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/TimeStamp.h"
#include "viewers/AdoptedViewer.h"
#include "viewers/CasesViewer.h"
#include "viewers/CliViewer.h"
#include "viewers/PersonsViewer.h"
#include "viewers/SummaryViewer.h"

#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {

CliController::CliController(const ptree& config_pt)
    : m_config_file(""), m_p_overrides(), m_stride_log_level(), m_use_install_dirs(), m_max_num_threads(1U),
      m_output_prefix(""), m_run_clock("run_clock", true), m_config_path(), m_config_pt(config_pt),
      m_stride_logger(nullptr)
{
        m_stride_log_level = m_config_pt.get<string>("run.stride_log_level");
        m_output_prefix    = m_config_pt.get<string>("run.output_prefix");
        m_use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");
};

void CliController::CheckEnv()
{
        if (m_use_install_dirs) {
                auto log = [](const string& s) -> void { cerr << s << endl; };
                if (!FileSys::CheckInstallEnv(log)) {
                        throw runtime_error("CliController::CheckEnv> Install dirs not OK.");
                }
        }
}

void CliController::CheckOpenMP() { m_max_num_threads = ConfigInfo::NumberAvailableThreads(); }

void CliController::CheckOutputPrefix()
{
        if (FileSys::IsDirectoryString(m_output_prefix)) {
                try {
                        create_directories(m_output_prefix);
                } catch (exception& e) {
                        cerr << "CliController::Setup> Exception creating directory:  {}" << m_output_prefix << endl;
                        throw;
                }
        }
}

void CliController::Control()
{
        // -----------------------------------------------------------------------------------------
        // Instantiate SimRunner & register viewers & setup+execute the run.
        // -----------------------------------------------------------------------------------------
        // Necessary (i.o. local variable) because (quote) a precondition of shared_from_this(),
        // namely that at least one shared_ptr must already have been created (and still exist)
        // pointing to this. Shared_from_this is used in viewer notification mechanism.
        // auto runner = make_shared<SimRunner>();
        auto runner = SimRunner::Create();

        // -----------------------------------------------------------------------------------------
        // Register viewers do runner setup and the execute.
        // -----------------------------------------------------------------------------------------
        RegisterViewers(runner);
        FileSys::WritePtreeFile(string("huhu.xml"), m_config_pt);
        runner->Setup(m_config_pt);
        runner->Run();
        m_stride_logger->info("CliController shutting down. Timing: {}", m_run_clock.ToString());

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        spdlog::drop_all();
}

void CliController::MakeLogger()
{
        const auto l    = FileSys::BuildPath(m_output_prefix, "stride_log.txt");
        m_stride_logger = LogUtils::CreateCliLogger("stride_logger", l.string());
        m_stride_logger->set_level(spdlog::level::from_str(m_stride_log_level));
        m_stride_logger->flush_on(spdlog::level::err);
}

void CliController::RegisterViewers(shared_ptr<SimRunner> runner)
{
        // Command line viewer
        m_stride_logger->info("Registering CliViewer");
        const auto cli_v = make_shared<viewers::CliViewer>(m_stride_logger);
        runner->Register(cli_v, bind(&viewers::CliViewer::Update, cli_v, placeholders::_1));

        // Adopted viewer
        if (m_config_pt.get<bool>("run.output_adopted", false)) {
                m_stride_logger->info("registering AdoptedViewer,");
                const auto v = make_shared<viewers::AdoptedViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::AdoptedViewer::Update, v, placeholders::_1));
        }

        // Cases viewer
        if (m_config_pt.get<bool>("run.output_cases", false)) {
                m_stride_logger->info("Registering CasesViewer");
                const auto v = make_shared<viewers::CasesViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::CasesViewer::Update, v, placeholders::_1));
        }

        // Persons viewer
        if (m_config_pt.get<bool>("run.output_persons", false)) {
                m_stride_logger->info("registering PersonsViewer.");
                const auto v = make_shared<viewers::PersonsViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::PersonsViewer::Update, v, placeholders::_1));
        }

        // Summary viewer
        if (m_config_pt.get<bool>("run.output_summary", false)) {
                m_stride_logger->info("Registering SummaryViewer");
                const auto v = make_shared<viewers::SummaryViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::SummaryViewer::Update, v, placeholders::_1));
        }
}

void CliController::Setup()
{
        // -----------------------------------------------------------------------------------------
        // Check environment, deal with output_prefix (i.e. make the directory iff the
        // prefix contains at least one /, make a logger and register it.
        // -----------------------------------------------------------------------------------------
        CheckEnv();
        CheckOpenMP();
        CheckOutputPrefix();
        MakeLogger();
        spdlog::register_logger(m_stride_logger);

        // -----------------------------------------------------------------------------------------
        // Log the setup.
        // -----------------------------------------------------------------------------------------
        m_stride_logger->info("CliController stating up at: {}", TimeStamp().ToString());
        m_stride_logger->info("Executing revision {}", ConfigInfo::GitRevision());
        m_stride_logger->info("Using configuration file:  {}", m_config_path.string());
        m_stride_logger->debug("Creating dir:  {}", m_output_prefix);
        m_stride_logger->debug("Executing:           {}", FileSys::GetExecPath().string());
        m_stride_logger->debug("Current directory:   {}", FileSys::GetCurrentDir().string());
        if (m_use_install_dirs) {
                m_stride_logger->debug("Install directory:   {}", FileSys::GetRootDir().string());
                m_stride_logger->debug("Config  directory:   {}", FileSys::GetConfigDir().string());
                m_stride_logger->debug("Data    directory:   {}", FileSys::GetDataDir().string());
        }
        if (ConfigInfo::HaveOpenMP()) {
                m_stride_logger->info("Max number OpenMP threads in this environment: {}", m_max_num_threads);
                m_stride_logger->info("Configured number of threads: {}",
                                      m_config_pt.get<unsigned int>("run.num_threads"));
        } else {
                m_stride_logger->info("Not using OpenMP threads.");
        }
}

} // namespace stride
