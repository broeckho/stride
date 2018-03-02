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
#include "sim/Simulator.h"

#include <iomanip>
#include <iostream>

using namespace std;

namespace stride {
namespace viewers {

void CliViewer::update(const sim_event::Payload& p)
{
        if (m_verbose) {
                cout << "stride>     step completed. ";
        }

        auto&      sim     = p.m_sim;
        const auto cases   = sim->GetPopulation()->GetInfectedCount();
        const auto adopted = sim->GetPopulation()->GetAdoptedCount();

        cout << "stride>         infected count: " << setw(7) << cases << "     Adopters count: " << setw(7) << adopted;
        cout << endl;
}

} // namespace viewers
} // namespace stride