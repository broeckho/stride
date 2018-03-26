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

#include <boost/property_tree/xml_parser.hpp>
#include <sstream>

using namespace stride::sim_event;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace std;

namespace stride {

SimRunner::SimRunner()
    : m_clock("total_clock"), m_stride_logger(nullptr), m_log_level("info"), m_output_prefix(""), m_config_pt(),
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
        m_config_pt     = run_config_pt;
        m_log_level     = m_config_pt.get<string>("run.stride_log_level", "info");
        m_output_prefix = m_config_pt.get<string>("run.output_prefix");

        // -----------------------------------------------------------------------------------------
        // Unless execution context has done so, create logger, do NOT register it.
        // -----------------------------------------------------------------------------------------
        m_stride_logger = spdlog::get("stride_logger");
        if (!m_stride_logger) {
                const auto l    = FileSys::BuildPath(m_output_prefix, "stride_log.txt");
                m_stride_logger = LogUtils::CreateFileLogger("stride_logger", l.string());
                m_stride_logger->set_level(spdlog::level::from_str(m_log_level));
                m_stride_logger->flush_on(spdlog::level::err);
        }
        m_stride_logger->info("SimRunner setup starting at: {}", TimeStamp().ToString());

        // -----------------------------------------------------------------------------------------
        // Output the full run config.
        // -----------------------------------------------------------------------------------------
        ostringstream ss;
        write_xml(ss, m_config_pt, xml_writer_make_settings<ptree::key_type>(' ', 8));
        m_stride_logger->debug("Run config used:\n {}", ss.str());

        // ------------------------------------------------------------------------------
        // Build simulator.
        //------------------------------------------------------------------------------
        m_stride_logger->trace("Building the simulator.");
        SimulatorBuilder builder(m_config_pt, m_stride_logger);
        m_sim = builder.Build();
        if (m_sim) {
                m_stride_logger->trace("Done building the simulator.");
        } else {
                m_stride_logger->critical("Simulator build failed!");
                throw("SimRunner::Setup> Simulator build failed!");
        }

        // -----------------------------------------------------------------------------------------
        // Check the simulator.
        // -----------------------------------------------------------------------------------------
        if (m_sim->IsOperational()) {
                m_stride_logger->trace("Simulator is operational.");
        } else {
                m_stride_logger->critical("Invalid configuration => terminate without output");
                throw("SimRunner::Setup> Simulator not operational!");
        }

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        m_clock.Stop();
        Notify({shared_from_this(), Id::SetupEnd});
        m_stride_logger->trace("Finished SimRunner::Setup.");
        return status;
}

void SimRunner::Run()
{
        // -----------------------------------------------------------------------------------------
        // Run the simulator.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        const auto num_days = m_config_pt.get<unsigned int>("run.num_days");
        m_stride_logger->info("SimRunner ready to simulate {} days:", num_days);

        Notify({shared_from_this(), Id::AtStart});
        for (unsigned int i = 0; i < num_days; i++) {
                m_sim->TimeStep();
                m_stride_logger->trace("Time step starting at day {} done.", i);
                Notify({shared_from_this(), Id::Stepped});
        }
        Notify({shared_from_this(), Id::Finished});

        m_clock.Stop();
        m_stride_logger->info("SimRunner done after: {}", m_clock.ToString());
}

} // namespace stride
