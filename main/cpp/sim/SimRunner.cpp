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
#include "util/TimeStamp.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

using namespace stride::sim_event;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace std;

namespace stride {

SimRunner::SimRunner()
    : m_clock("total_clock"), m_logger(nullptr), m_operational(false), m_output_prefix(""), m_pt_config(),
      m_sim(nullptr)
{
}

bool SimRunner::Setup(const ptree& run_config_pt)
{
        Notify({shared_from_this(), Id::SetupBegin});

        // -----------------------------------------------------------------------------------------
        // Intro.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        bool status     = true;
        m_pt_config     = run_config_pt;
        m_output_prefix = m_pt_config.get<string>("run.output_prefix");

        // -----------------------------------------------------------------------------------------
        // Unless execution context has done so, create logger, do not register it.
        // -----------------------------------------------------------------------------------------
        m_logger = spdlog::get("run_logger");
        if (!m_logger) {
                const auto p = FileSys::BuildPath(m_output_prefix, "run_log.txt");
                m_logger     = LogUtils::CreateFileLogger("run_logger", p.string());
        }
        m_logger->info("SimRunner starting up at:      {}", TimeStamp().ToString());

        // -----------------------------------------------------------------------------------------
        // Output run config.
        // -----------------------------------------------------------------------------------------
        const auto p = FileSys::BuildPath(m_output_prefix, "run_config.xml");
        write_xml(p.string(), m_pt_config, std::locale(), xml_writer_make_settings<ptree::key_type>(' ', 8));
        m_logger->info("Run config written to file {}", p.string());

        // ------------------------------------------------------------------------------
        // Build simulator.
        //------------------------------------------------------------------------------
        m_logger->info("Building the simulator.");
        SimulatorBuilder builder(m_pt_config, m_logger);
        m_sim = builder.Build();
        if (m_sim) {
                m_logger->info("Done building the simulator.");
        } else {
                m_logger->critical("Simulator build failed!");
                throw("SimRunner::Setup> Simulator build failed!");
        }

        // -----------------------------------------------------------------------------------------
        // Check the simulator.
        // -----------------------------------------------------------------------------------------
        if (m_sim->IsOperational()) {
                m_logger->info("Simulator is operational.");
        } else {
                m_logger->critical("Invalid configuration => terminate without output");
                throw("SimRunner::Setup> Simulator not operational!");
        }

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        m_clock.Stop();
        Notify({shared_from_this(), Id::SetupEnd});
        m_logger->info("SimRunner setup ok.");
        return status;
}

void SimRunner::Run()
{
        // -----------------------------------------------------------------------------------------
        // Run the simulator.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        const auto num_days = m_pt_config.get<unsigned int>("run.num_days");
        m_logger->info("SimRunner ready to run for {} days:", num_days);

        Notify({shared_from_this(), Id::AtStart});
        for (unsigned int i = 0; i < num_days; i++) {
                m_sim->TimeStep();
                m_logger->info("Time step starting at day {} done.", i);
                Notify({shared_from_this(), Id::Stepped});
        }
        Notify({shared_from_this(), Id::Finished});

        m_clock.Stop();
        m_logger->info("elapsed time on this run; {}", m_clock.ToString());
        m_logger->info("SimRunner shutting down at:      {}", TimeStamp().ToString());
}

} // namespace stride
