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
 * Observer for SimEvents for commandline interface usage.
 */

#include "InfectedViewer.h"

#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"

using namespace std;
using namespace stride::sim_event;

namespace stride {
namespace viewers {

void InfectedViewer::Update(const sim_event::Id id)
{
        switch (id) {
        case Id::AtStart:
        case Id::Stepped: {
                const auto pop = m_runner->GetSim()->GetPopulation();
                m_infected.push_back(pop->GetInfectedCount());
                break;
        }
        default: break;
        }
}

} // namespace viewers
} // namespace stride
