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
        // Output_prefix.
        // -----------------------------------------------------------------------------------------
        const auto output_prefix = m_config_pt.get<string>("run.output_prefix");
        // If it's a string not containing any / it gets interpreted as a filename prefix.
        // Otherwise we 'll see to it that the corresponding directory exists.
        if (FileSys::IsDirectoryString(output_prefix)) {
                boost::filesystem::path out_dir = output_prefix;
                try {
                        m_logger->info("Creating dir:  {}", out_dir.string());
                        create_directories(out_dir);
                } catch (std::exception& e) {
                        m_logger->info("CliController::Go> Exception while creating output directory:  {}", e.what());
                        throw;
                }
                m_logger->info("Dir created:  {}", out_dir.string());
        }

        // -----------------------------------------------------------------------------------------
        // Register viewers.
        // -----------------------------------------------------------------------------------------
        RegisterViewers(runner, output_prefix);

        // -----------------------------------------------------------------------------------------
        // Setup runner + execute the run.
        // -----------------------------------------------------------------------------------------
        m_logger->info("Handing over to SimRunner.");
        // TODO for now to check up things
        const auto p = FileSys::BuildPath(output_prefix, "run_config.xml");
        write_xml(p.string(), m_config_pt, std::locale(),
                  xml_parser::xml_writer_make_settings<ptree::key_type>(' ', 8));
        runner->Setup(m_config_pt);
        runner->Run();
        m_logger->info("SimRun completed. Timing: {}", m_run_clock.ToString());

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        spdlog::drop_all();
        m_logger->info("CliController signing off.");
}

void CliController::RegisterViewers(shared_ptr<SimRunner> runner, const string& output_prefix)
{
        m_logger->info("Registering viewers.");

        // Command line viewer
        const auto cli_v = make_shared<viewers::CliViewer>(m_logger);
        runner->Register(cli_v, bind(&viewers::CliViewer::update, cli_v, placeholders::_1));

        // Adopted viewer
        if (m_config_pt.get<bool>("run.output_adopted", false)) {
                m_logger->info("Setting for output of adopted: true");
                const auto v = make_shared<viewers::AdoptedViewer>(output_prefix);
                runner->Register(v, bind(&viewers::AdoptedViewer::update, v, placeholders::_1));
        } else {
                m_logger->info("Setting for output of adopted: false");
        }

        // Cases viewer
        if (m_config_pt.get<bool>("run.output_cases", false)) {
                m_logger->info("Setting for output of cases: true");
                const auto v = make_shared<viewers::CasesViewer>(output_prefix);
                runner->Register(v, bind(&viewers::CasesViewer::update, v, placeholders::_1));
        } else {
                m_logger->info("Setting for output of cases: false");
        }

        // Persons viewer
        if (m_config_pt.get<bool>("run.output_persons", false)) {
                m_logger->info("Setting for output of persons: true");
                const auto v = make_shared<viewers::PersonsViewer>(output_prefix);
                runner->Register(v, bind(&viewers::PersonsViewer::update, v, placeholders::_1));
        } else {
                m_logger->info("Setting for output of persons: false");
        }

        // Summary viewer
        if (m_config_pt.get<bool>("run.output_summary", false)) {
                m_logger->info("Setting for output of summary: true");
                const auto v = make_shared<viewers::SummaryViewer>(output_prefix);
                runner->Register(v, bind(&viewers::SummaryViewer::update, v, placeholders::_1));
        } else {
                m_logger->info("Setting for output of summary: false");
        }
}

void CliController::Setup()
{
        m_logger = m_silent_mode ? LogUtils::GetNullLogger("stride_logger")
                                 : LogUtils::GetCliLogger("stride_logger", "stride_log.txt");
        m_logger->info("CliController setup:");
        m_logger->info("Starting up at:      {}", TimeStamp().ToString());
        CheckEnv();
        CheckOpenMP();
        SetupConfig();
}

void CliController::SetupConfig()
{
        // -----------------------------------------------------------------------------------------
        // Read the config file.
        // -----------------------------------------------------------------------------------------
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
        m_logger->info("Setting for run.track_index_case:  {}", m_track_index_case);

        m_config_pt.put("run.silent_mode", m_silent_mode);
        m_logger->info("Setting for run.silent_mode:  {}", m_silent_mode);

        m_config_pt.put("run.use_install_dirs", m_use_install_dirs);
        m_logger->info("Setting for run.use_install_dirs:  {}", m_use_install_dirs);

        // -----------------------------------------------------------------------------------------
        // Config item that is often defaulted: num_threads.
        // -----------------------------------------------------------------------------------------
        const auto opt_num = m_config_pt.get_optional<unsigned int>("run.num_threads");
        if (!opt_num) {
                // default for num_threads if not specified in config or commandline
                m_config_pt.put("run.num_threads", m_max_num_threads);
                m_logger->info("Default for run.num_threads:  {}", m_max_num_threads);
        } else {
                m_logger->info("Specification for run.num_threads:  {}", *opt_num);
        }

        // -----------------------------------------------------------------------------------------
        // Config item that is often defaulted: output_prefix.
        // -----------------------------------------------------------------------------------------
        auto output_prefix = m_config_pt.get<string>("run.output_prefix", "");
        if (output_prefix.length() == 0) {
                // Not specified with (-p output_prefix=<prefix>) or in config, so default
                output_prefix = TimeStamp().ToTag() + "/";
                m_config_pt.put("run.output_prefix", output_prefix);
                m_logger->info("Default for run.output_prefix:  {}", output_prefix);
        } else {
                m_logger->info("Specification for run.output_prefix:  {}", output_prefix);
        }
}

std::shared_ptr<spdlog::logger> CliController::SetupLogger()
{
        using namespace spdlog;

        set_async_mode(1048576);
        std::shared_ptr<spdlog::logger> stride_logger = nullptr;
        try {
                vector<sink_ptr> sinks;
                const auto       color_sink = make_shared<sinks::ansicolor_stdout_sink_st>();
                sinks.push_back(color_sink);
                const string fn = "stride_log.txt";
                sinks.push_back(make_shared<sinks::simple_file_sink_st>(fn.c_str()));
                stride_logger = make_shared<logger>("stride_logger", begin(sinks), end(sinks));
                register_logger(stride_logger);
        } catch (const spdlog_ex& e) {
                cerr << "CliController> Stride logger initialization failed: " << e.what() << endl;
                throw;
        }
        return stride_logger;
}

std::shared_ptr<spdlog::logger> CliController::SetupNullLogger()
{
        using namespace spdlog;
        ;
        set_async_mode(1048576);
        std::shared_ptr<spdlog::logger> stride_logger = nullptr;
        try {
                const auto null_sink = make_shared<sinks::null_sink_st>();
                stride_logger        = make_shared<logger>("stride_logger", null_sink);
                register_logger(stride_logger);
        } catch (const spdlog_ex& e) {
                cerr << "CliController> Stride null logger initialization failed: " << e.what() << endl;
                throw;
        }
        return stride_logger;
}

} // namespace stride