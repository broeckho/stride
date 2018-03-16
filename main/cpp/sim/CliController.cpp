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
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/StringUtils.h"
#include "util/TimeStamp.h"
#include "viewers/AdoptedViewer.h"
#include "viewers/CasesViewer.h"
#include "viewers/CliViewer.h"
#include "viewers/PersonsViewer.h"
#include "viewers/SummaryViewer.h"

#include <boost/property_tree/xml_parser.hpp>
#include <memory>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>
#include <vector>

using namespace std;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {

void CliController::CheckEnv()
{
        FileSys dirs;
        m_logger->info("Executing:           {}", dirs.GetExecPath().string());
        m_logger->info("Current directory:   {}", dirs.GetCurrentDir().string());

        if (m_use_install_dirs) {
                auto log = [l = this->m_logger](const string& s)->void { l->critical(s); };
                if (!FileSys::CheckInstallEnv(log)) {
                        throw std::runtime_error("CliController::CheckEnv> Install dirs not OK.");
                } else {
                        m_logger->info("Install directory:   {}", dirs.GetRootDir().string());
                        m_logger->info("Config  directory:   {}", dirs.GetConfigDir().string());
                        m_logger->info("Data    directory:   {}", dirs.GetDataDir().string());
                }
        }
}

void CliController::CheckOpenMP()
{
        m_max_num_threads = ConfigInfo::NumberAvailableThreads();
        if (ConfigInfo::HaveOpenMP()) {
                m_logger->info("Max number OpenMP threads in this environment: {}", m_max_num_threads);
        } else {
                m_logger->info("Not using OpenMP threads.");
        }
}

void CliController::Go()
{
        // -----------------------------------------------------------------------------------------
        // Instantiate SimRunner & register viewers & setup+execute the run.
        // -----------------------------------------------------------------------------------------
        // Necessary (i.o. local variable) because (quote) a precondition of shared_from_this(),
        // namely that at least one shared_ptr must already have been created (and still exist)
        // pointing to this. Shared_from_this is used in viewer notification mechanism.
        auto runner = make_shared<SimRunner>();

        // -----------------------------------------------------------------------------------------
        // Output_prefix: if it's a string not containing any / it gets interpreted as a
        // filename prefix; otherwise we 'll create the corresponding directory.
        // -----------------------------------------------------------------------------------------
        const auto output_prefix = m_config_pt.get<string>("run.output_prefix");
        if (FileSys::IsDirectoryString(output_prefix)) {
                try {
                        m_logger->info("Creating dir:  {}", output_prefix);
                        create_directories(output_prefix);
                } catch (std::exception& e) {
                        m_logger->critical("CliController::Go> Exception creating directory:  {}", output_prefix);
                        throw;
                }
        }

        // -----------------------------------------------------------------------------------------
        // Register viewers.
        // -----------------------------------------------------------------------------------------
        RegisterViewers(runner, output_prefix);

        // -----------------------------------------------------------------------------------------
        // Setup runner + execute the run.
        // -----------------------------------------------------------------------------------------
        runner->Setup(m_config_pt);
        runner->Run();
        m_logger->info("CliController shutting down. Timing: {}", m_run_clock.ToString());

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        spdlog::drop_all();
}

void CliController::PatchConfig()
{
        // -----------------------------------------------------------------------------------------
        // Overrides/additions for configuration (using -p <param>=<value>) on commandline.
        // -----------------------------------------------------------------------------------------
        for (const auto& p : m_p_overrides) {
                m_config_pt.put("run." + get<0>(p), get<1>(p));
                m_logger->info("Commanline override for run.{}:  {}", get<0>(p), get<1>(p));
        }

        // -----------------------------------------------------------------------------------------
        // Config items that can ONLY specified with commandline options (-r, -s, -w NOT with -p)
        // -----------------------------------------------------------------------------------------
        m_config_pt.put("run.track_index_case", m_track_index_case);
        m_config_pt.put("run.silent_mode", m_silent_mode);
        m_config_pt.put("run.use_install_dirs", m_use_install_dirs);

        // -----------------------------------------------------------------------------------------
        // Config item that is often defaulted: num_threads.
        // -----------------------------------------------------------------------------------------
        const auto opt_num = m_config_pt.get_optional<unsigned int>("run.num_threads");
        if (!opt_num) {
                // default for num_threads if not specified in config or commandline
                m_config_pt.put("run.num_threads", m_max_num_threads);
        }

        // -----------------------------------------------------------------------------------------
        // Config item that is often defaulted: output_prefix.
        // -----------------------------------------------------------------------------------------
        auto output_prefix = m_config_pt.get<string>("run.output_prefix", "");
        if (output_prefix.length() == 0) {
                // Not specified with (-p output_prefix=<prefix>) or in config, so default
                output_prefix = TimeStamp().ToTag() + "/";
                m_config_pt.put("run.output_prefix", output_prefix);
        }
}

void CliController::ReadConfigFile()
{
        const auto file_path =
            (m_use_install_dirs) ? FileSys().GetConfigDir() /= m_config_file : system_complete(m_config_file);
        if (!exists(file_path) || !is_regular_file(file_path)) {
                m_logger->critical("Configuration file {} not present! Quitting.", file_path.string());
                throw;
        } else {
                m_logger->info("Configuration file:  {}", file_path.string());
                try {
                        read_xml(canonical(file_path).string(), m_config_pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        m_logger->critical("Error reading {}\nException: {}", canonical(file_path).string(), e.what());
                        throw;
                }
        }
}

void CliController::RegisterViewers(shared_ptr<SimRunner> runner, const string& output_prefix)
{
        // Command line viewer
        m_logger->info("Registering CliViewer");
        const auto cli_v = make_shared<viewers::CliViewer>(m_logger);
        runner->Register(cli_v, bind(&viewers::CliViewer::update, cli_v, placeholders::_1));

        // Adopted viewer
        if (m_config_pt.get<bool>("run.output_adopted", false)) {
                m_logger->info("registering AdoptedViewer,");
                const auto v = make_shared<viewers::AdoptedViewer>(output_prefix);
                runner->Register(v, bind(&viewers::AdoptedViewer::update, v, placeholders::_1));
        }

        // Cases viewer
        if (m_config_pt.get<bool>("run.output_cases", false)) {
                m_logger->info("Registering CasesViewer");
                const auto v = make_shared<viewers::CasesViewer>(output_prefix);
                runner->Register(v, bind(&viewers::CasesViewer::update, v, placeholders::_1));
        }

        // Persons viewer
        if (m_config_pt.get<bool>("run.output_persons", false)) {
                m_logger->info("registering PersonsViewer.");
                const auto v = make_shared<viewers::PersonsViewer>(output_prefix);
                runner->Register(v, bind(&viewers::PersonsViewer::update, v, placeholders::_1));
        }

        // Summary viewer
        if (m_config_pt.get<bool>("run.output_summary", false)) {
                m_logger->info("Registering SummaryViewer");
                const auto v = make_shared<viewers::SummaryViewer>(output_prefix);
                runner->Register(v, bind(&viewers::SummaryViewer::update, v, placeholders::_1));
        }
}

void CliController::Setup()
{
        // -----------------------------------------------------------------------------------------
        // Create the appropriate logger and register it.
        // -----------------------------------------------------------------------------------------
        const string log_f = "/tmp/stride_log_" + TimeStamp().ToTag() + ".txt";
        m_logger           = m_silent_mode ? LogUtils::CreateNullLogger("stride_logger")
                                 : LogUtils::CreateCliLogger("stride_logger", log_f);
        spdlog::register_logger(m_logger);

        // -----------------------------------------------------------------------------------------
        // Do the setup.
        // -----------------------------------------------------------------------------------------
        m_logger->info("CliController setup starting up at:      {}", TimeStamp().ToString());
        CheckEnv();
        CheckOpenMP();
        ReadConfigFile();
        PatchConfig();
}

} // namespace stride