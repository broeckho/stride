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

#include "calendar/Calendar.h"
#include "pop/Population.h"
#include "sim/Sim.h"

using namespace stride::sim_event;
using namespace boost::property_tree;
using namespace std;

namespace stride {

SimRunner::SimRunner(const ptree& configPt, shared_ptr<Population> pop, util::RnMan& rnManager)
    : m_clock("total_clock"), m_config(configPt), m_output_prefix(""), m_sim(nullptr)
{
        m_clock.Start();
        Notify(Id::SetupBegin);

        m_output_prefix = m_config.get<string>("run.output_prefix");
        m_sim           = Sim::Create(m_config, std::move(pop), rnManager);

        Notify(Id::SetupEnd);
        m_clock.Stop();
}

void SimRunner::Run(unsigned int numSteps)
{
        // Saveguard against repeatedly firing AtStart, so bypass if numSteps == 0.
        if (numSteps != 0U) {
                // Prelims.
                m_clock.Start();
                const auto numDays = m_config.get<unsigned int>("run.num_days");

                // We are AtStart: no steps have taken yet, so signal AtStart.
                if (m_sim->GetCalendar()->GetSimulationDay() == 0) {
                        Notify(Id::AtStart);
                }

                // Tahe numSteps but do not go beyond numDays.
                for (unsigned int i = 0; i < numSteps; i++) {
                        // This is not the last step: execute and signal Stepped.
                        if (m_sim->GetCalendar()->GetSimulationDay() < numDays - 1) {
                                m_sim->TimeStep();
                                Notify(Id::Stepped);
                                // This is the last step so execute and afterwards signal Stepped and Finished
                        } else if (m_sim->GetCalendar()->GetSimulationDay() == numDays - 1) {
                                m_sim->TimeStep();
                                Notify(Id::Stepped);
                                Notify(Id::Finished);
                                break;
                                // We are apparently already at the end of the numDays so nothing to do or signal.
                        } else {
                                break;
                        }
                }

                m_clock.Stop();
        }
}

void SimRunner::Run() { Run(m_config.get<unsigned int>("run.num_days")); }

} // namespace stride
