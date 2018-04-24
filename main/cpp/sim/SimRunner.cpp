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

SimRunner::SimRunner() : m_clock("total_clock"), m_output_prefix(""), m_config_pt(), m_sim(nullptr) {}

void SimRunner::Setup(const ptree& config_pt)
{
        // -----------------------------------------------------------------------------------------
        // Intro.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        Notify(Id::SetupBegin);
        m_config_pt     = config_pt;
        m_output_prefix = m_config_pt.get<string>("run.output_prefix");

        // -----------------------------------------------------------------------------------------
        // Build simulator.
        //------------------------------------------------------------------------------------------
        SimBuilder builder(m_config_pt);
        m_sim = builder.Build();

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        m_clock.Stop();
        Notify(Id::SetupEnd);
}

void SimRunner::AtFinish()
{
        m_clock.Start();
        Notify(Id::Finished);
        m_clock.Stop();
}

void SimRunner::AtStart()
{
        m_clock.Start();
        Notify(Id::AtStart);
        m_clock.Stop();
}

void SimRunner::Run(unsigned int numDays)
{
        m_clock.Start();
        for (unsigned int i = 0; i < numDays; i++) {
                m_sim->TimeStep();
                Notify(Id::Stepped);
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
