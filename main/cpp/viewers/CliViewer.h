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
 * Observer for SimEvents for commandline interface usage.
 */

#include "sim/event/Payload.h"

#include <spdlog/spdlog.h>

namespace stride {
namespace viewers {

/// Viewer of Simulator for commandline interface.
class CliViewer
{
public:
        /// Instantiate cli viewer.
        explicit CliViewer(std::shared_ptr<spdlog::logger> logger) : m_logger(logger) {}

        /// Let viewer perform update.
        void update(const sim_event::Payload& p);

private:
        std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace viewers
} // namespace stride
