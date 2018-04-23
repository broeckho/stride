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

#include "pop/PopBuilder.h"
#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimBuilder.h"
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
    : m_clock("total_clock"), m_output_prefix(""), m_config_pt(), m_sim(nullptr), m_stride_logger(nullptr)
{
}

void SimRunner::Setup(const ptree& config_pt, std::shared_ptr<spdlog::logger> logger)
{
        // -----------------------------------------------------------------------------------------
        // Intro.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        Notify({shared_from_this(), Id::SetupBegin});
        m_config_pt     = config_pt;
        m_output_prefix = m_config_pt.get<string>("run.output_prefix");

        // -----------------------------------------------------------------------------------------
        // C.
        // -----------------------------------------------------------------------------------------
        m_stride_logger = std::move(logger);
        if (!m_stride_logger) {
                // So as not to have to guard all log statements
                m_stride_logger = LogUtils::CreateNullLogger("NullLogger");
        }

        // -----------------------------------------------------------------------------------------
        // Build simulator.
        //------------------------------------------------------------------------------------------
        m_stride_logger->trace("Starting SimBuilder::Build.");
        SimBuilder builder(m_config_pt, m_stride_logger);
        m_stride_logger->trace("Finished SimBuilder::Build.");
        m_sim = builder.Build();

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        m_clock.Stop();
        Notify({shared_from_this(), Id::SetupEnd});
}

void SimRunner::AtFinish()
{
        m_clock.Start();
        Notify({shared_from_this(), Id::Finished});
        m_clock.Stop();
}

void SimRunner::AtStart()
{
        m_clock.Start();
        Notify({shared_from_this(), Id::AtStart});
        m_clock.Stop();
}

void SimRunner::Run(unsigned int numDays)
{
        m_clock.Start();
        for (unsigned int i = 0; i < numDays; i++) {
                m_sim->TimeStep();
                Notify({shared_from_this(), Id::Stepped});
        }
        m_clock.Stop();
}

void SimRunner::Run()
{
        AtStart();
        const auto numDays = m_config_pt.get<unsigned int>("run.num_days");
        Run(numDays);
        AtFinish();
}

} // namespace stride
