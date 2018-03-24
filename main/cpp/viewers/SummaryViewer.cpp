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

#include "SummaryViewer.h"

#include "pop/Population.h"
#include "sim/SimRunner.h"
#include "sim/Simulator.h"

using namespace std;
using namespace std::chrono;
using namespace stride::sim_event;

namespace stride {
namespace viewers {

void SummaryViewer::update(const sim_event::Payload& p)
{
        switch (p.m_event_id) {
        case Id::Finished: {
                const auto pt_config = p.m_runner->GetConfig();
                const auto sim       = p.m_runner->GetSim();
                const auto pop       = p.m_runner->GetSim()->GetPopulation();
                const auto dur       = duration_cast<milliseconds>(p.m_runner->GetClock().Get());
                const auto milli     = static_cast<unsigned int>(dur.count());
                m_summary_file.Print(pt_config, static_cast<unsigned int>(pop->size()), pop->GetInfectedCount(),
                                     sim->GetDiseaseProfile().GetTransmissionRate(), milli, milli);
                break;
        }
        default: break;
        }
}

} // namespace viewers
} // namespace stride
