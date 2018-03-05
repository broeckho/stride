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
 * Implementation for SimRunner.
 */

#include "SimRunner.h"

#include "sim/Simulator.h"
#include "sim/SimulatorBuilder.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <spdlog/sinks/null_sink.h>

namespace stride {

using namespace sim_event;
using namespace util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace spdlog;
using namespace std;

SimRunner::SimRunner()
    : m_clock("total_clock"), m_logger(nullptr), m_operational(false), m_output_prefix(""), m_pt_config(),
      m_sim(nullptr)
{
}

bool SimRunner::Setup(const ptree& run_config_pt)
{
        // -----------------------------------------------------------------------------------------
        // Intro.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        bool status = true;
        m_pt_config = run_config_pt;
        m_logger    = spdlog::get("stride_logger");
        if (!m_logger) {
                m_logger = m_pt_config.get<bool>("run.silent_mode")
                               ? LogUtils::GetNullLogger("stride_logger")
                               : LogUtils::GetCliLogger("stride_logger", "stride_log.txt");
        }
        m_output_prefix = m_pt_config.get<string>("run.output_prefix");

        // -----------------------------------------------------------------------------------------
        // Create logger for use by the simulator during time step computations.
        // Transmissions: [TRANSMISSION] <infecterID> <infectedID> <contactpoolID> <day>
        // Contacts: [CNT] <person1ID> <person1AGE> <person2AGE> <at_home> <at_work> <at_school> <at_other>
        // -----------------------------------------------------------------------------------------
        spdlog::set_async_mode(1048576);
        const auto log_path       = FileSys::BuildPath(m_output_prefix, "contact_log.txt");
        auto       contact_logger = spdlog::rotating_logger_mt("contact_logger", log_path.c_str(),
                                                         numeric_limits<size_t>::max(), numeric_limits<size_t>::max());
        contact_logger->set_pattern("%v"); // Remove meta data from log => time-stamp of logging

        // ------------------------------------------------------------------------------
        // Create the simulator builder.
        //------------------------------------------------------------------------------
        m_logger->info("Creating the simulator builder");
        SimulatorBuilder builder(m_pt_config);
        m_logger->info("Done creating the simulator builder");

        // ------------------------------------------------------------------------------
        // Build simulator.
        //------------------------------------------------------------------------------
        m_logger->info("Building the simulator.");
        m_sim = builder.Build();
        m_logger->info("Done building the simulator.");

        // -----------------------------------------------------------------------------------------
        // Check the simulator.
        // -----------------------------------------------------------------------------------------
        if (!m_sim) {
                m_logger->critical("Simulation build failed!");
                status = false;
        } else {
                if (m_sim->IsOperational()) {
                        m_logger->info("Done checking the simulator. OK.");
                } else {
                        m_logger->critical("Invalid configuration => terminate without output");
                        status = false;
                }
        }

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        m_clock.Stop();
        return status;
}

void SimRunner::Run()
{
        // -----------------------------------------------------------------------------------------
        // Run the simulator.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        const auto num_days = m_pt_config.get<unsigned int>("run.num_days");
        Notify({shared_from_this(), Id::AtStart});
        for (unsigned int i = 0; i < num_days; i++) {
                m_sim->TimeStep();
                Notify({shared_from_this(), Id::Stepped});
        }
        Notify({shared_from_this(), Id::Finished});
        m_clock.Stop();
}

} // namespace stride
