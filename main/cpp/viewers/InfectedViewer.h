#pragma once
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
 * Observer for infection count at each sim step.
 */

#include "sim/SimRunner.h"
#include "sim/event/Id.h"

#include <vector>

namespace stride {
namespace viewers {

/// Viewer gathers infection count at each sim step.
class InfectedViewer
{
public:
        /// Instantiate cases viewer.
        InfectedViewer(std::shared_ptr<SimRunner> runner) : m_infected(), m_runner(std::move(runner)) {}

        std::vector<unsigned int> GetInfectionCounts() { return m_infected; }

        /// Let viewer perform update.
        void Update(sim_event::Id id);

private:
        std::vector<unsigned int>  m_infected;
        std::shared_ptr<SimRunner> m_runner;
};

} // namespace viewers
} // namespace stride
