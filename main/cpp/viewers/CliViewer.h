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

namespace stride {
namespace viewers {

/// Viewer of Simulayor for commandline interface.
class CliViewer
{
public:
        /// Instatntiate cli viewer.
        /// \param verbose      Yes/no verbose.
        CliViewer(bool verbose) : m_verbose(verbose) {}

        /// Let viewer perform update.
        void update(const sim_event::Payload& p);

        /// Switch off verbosity.
        void SetVerboseOff() { m_verbose = false; }

        /// Switch on verbosity.
        void SetVerboseOn() { m_verbose = true; }

private:
        bool m_verbose;
};

} // namespace viewers
} // namespace stride