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
 * Implementation for StrideRunner.
 */

#include "StrideRunner.h"

#include "calendar/Calendar.h"
#include "output/AdoptedFile.h"
#include "output/CasesFile.h"
#include "output/PersonsFile.h"
#include "output/SummaryFile.h"
#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimBuilder.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/StringUtils.h"
#include "util/TimeStamp.h"

#include <boost/property_tree/xml_parser.hpp>
#include <omp.h>

namespace stride {
namespace python {

using namespace output;
using namespace util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace std;
using namespace std::chrono;

StrideRunner::StrideRunner()
    : m_is_running(false), m_output_prefix(""), m_pt_config(), m_clock("total_clock"), m_sim(nullptr)
{
}

void StrideRunner::RegisterObserver(std::shared_ptr<python::SimulatorObserver>& observer) { m_sim->Register(observer); }

void StrideRunner::Setup(bool track_index_case, const string& config_file_name, bool use_install_dirs)
{
        // -----------------------------------------------------------------------------------------
        // Print output to command line.
        // -----------------------------------------------------------------------------------------
        cout << "\n*************************************************************" << endl;
        cout << "Starting up at:      " << TimeStamp().ToString() << endl;

        if (use_install_dirs) {
                cout << "Executing:           " << FileSys::GetExecPath().string() << endl;
                cout << "Current directory:   " << FileSys::GetCurrentDir().string() << endl;
                cout << "Install directory:   " << FileSys::GetRootDir().string() << endl;
                cout << "Data    directory:   " << FileSys::GetDataDir().string() << endl;

                // -----------------------------------------------------------------------------------------
                // Check execution environment.
                // -----------------------------------------------------------------------------------------
                if (FileSys::GetCurrentDir().compare(FileSys::GetRootDir()) != 0) {
                        throw runtime_error(string(__func__) + "> Current directory is not install root! Aborting.");
                }
                if (FileSys::GetDataDir().empty()) {
                        throw runtime_error(string(__func__) + "> Data directory not present! Aborting.");
                }
        }

        // -----------------------------------------------------------------------------------------
        // Configuration.
        // -----------------------------------------------------------------------------------------
        const auto file_path = canonical(system_complete(config_file_name));
        if (!is_regular_file(file_path)) {
                throw runtime_error(string(__func__) + ">Config file " + file_path.string() +
                                    " not present. Aborting.");
        }
        read_xml(file_path.string(), m_pt_config);
        cout << "Configuration file:  " << file_path.string() << endl;

        // -----------------------------------------------------------------------------------------
        // OpenMP.
        // -----------------------------------------------------------------------------------------
        unsigned int num_threads;

#pragma omp parallel
        {
                num_threads = static_cast<unsigned int>(omp_get_num_threads());
        }
        if (ConfigInfo::HaveOpenMP()) {
                cout << "Using OpenMP threads:  " << num_threads << endl;
        } else {
                cout << "Not using OpenMP threads." << endl;
        }

        // -----------------------------------------------------------------------------------------
        // Configuration.
        // -----------------------------------------------------------------------------------------
        // track_index_case (-r switch on commandline)
        m_pt_config.put("run.track_index_case", track_index_case);

        // use_install_dirs (-w or --working_dir switch on commandline)
        m_pt_config.put("run.use_install_dirs", use_install_dirs);

        // num_threads
        m_pt_config.put("run.num_threads", num_threads);

        // -----------------------------------------------------------------------------------------
        // Set output path prefix.
        // -----------------------------------------------------------------------------------------
        m_output_prefix = m_pt_config.get<string>("run.output_prefix", "");
        if (m_output_prefix.length() == 0) {
                m_output_prefix = TimeStamp().ToTag();
        }
        cout << "Project output tag:  " << m_output_prefix << endl << endl;

        // -----------------------------------------------------------------------------------------
        // Additional run configurations.
        // -----------------------------------------------------------------------------------------
        if (m_pt_config.get_optional<bool>("run.num_participants_survey") == false) {
                m_pt_config.put("run.num_participants_survey", 1);
        }

        // ------------------------------------------------------------------------------
        // Create the simulator.
        //------------------------------------------------------------------------------
        m_clock.Start();
        cout << "Building the simulator. " << endl;
        m_sim = Sim::Create(m_pt_config);
        cout << "Done building the simulator. " << endl;
}

/// Run the simulator with config information provided.
void StrideRunner::Run()
{

        // -----------------------------------------------------------------------------------------
        // Run the simulation (if operational).
        // -----------------------------------------------------------------------------------------
        Stopwatch<> run_clock("run_clock");

        m_is_running = true;
        const auto num_days{m_pt_config.get<unsigned int>("run.num_days")};
        std::cout << "Starting the run for " << num_days << " days." << std::endl;
        for (unsigned int i = 0; i < num_days; i++) {
                // Check if still running
                if (!m_is_running) {
                        break;
                }

                run_clock.Start();
                m_sim->TimeStep();
                run_clock.Stop();
                m_sim->Notify(static_cast<unsigned int>(m_sim->GetCalendar()->GetSimulationDay()));
        }

        m_is_running = false;

        // Clean up
        spdlog::drop_all();

        // -----------------------------------------------------------------------------------------
        // Print final message to command line.
        // -----------------------------------------------------------------------------------------
        cout << endl << endl;
        cout << "  run_time: " << run_clock.ToString() << "  -- total time: " << m_clock.ToString() << endl;
        cout << "Exiting at:         " << TimeStamp().ToString() << endl << endl;
}

void StrideRunner::Stop() { m_is_running = false; }

} // namespace python
} // namespace stride
