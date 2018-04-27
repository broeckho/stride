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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Definition of Observer for SimEvents for commandline interface usage.
 */

#include "CliViewer.h"

#include "calendar/Calendar.h"
#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"

#include <boost/property_tree/xml_parser.hpp>
#include <sstream>

using namespace std;
using namespace stride::sim_event;
using namespace stride::util;
using namespace boost::property_tree;

namespace stride {
namespace viewers {

void CliViewer::Update(const sim_event::Id id)
{
        switch (id) {
        case Id::AtStart: {
                const auto sim = m_runner->GetSim();
                m_logger->info("   SimRunner at start:");
                ostringstream ss;
                write_xml(ss, m_runner->GetConfig(), xml_writer_make_settings<ptree::key_type>(' ', 8));
                m_logger->trace("Run config used:\n {}", ss.str());
                m_logger->info("      Day: {:4}  Done, infected count: {:7}", sim->GetCalendar()->GetSimulationDay(),
                               sim->GetPopulation()->GetInfectedCount());
                break;
        }
        case Id::Stepped: {
                const auto sim = m_runner->GetSim();
                m_logger->info("      Day: {:4}  Done, infected count: {:7}", sim->GetCalendar()->GetSimulationDay(),
                               sim->GetPopulation()->GetInfectedCount());
                break;
        }
        case Id::Finished: {
                const auto sim = m_runner->GetSim();
                m_logger->info("   SimRunner done after: {}", m_runner->GetClock().ToString());
                break;
        }
        default: break;
        }
}

} // namespace viewers
} // namespace stride
