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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Payload for Subject/Observer for SimEvents.
 */

#include "Id.h"

#include <memory>

namespace stride {

class SimRunner;

namespace sim_event {

struct Payload
{
        Payload(std::shared_ptr<SimRunner> s, sim_event::Id e) : m_runner(s), m_event_id(e){};
        std::shared_ptr<SimRunner> m_runner;
        sim_event::Id              m_event_id;
};

} // namespace sim_event
} // namespace stride
