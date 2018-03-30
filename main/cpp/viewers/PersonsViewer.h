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
 * Observer for Persons output.
 */

#include "output/PersonsFile.h"
#include "sim/event/Payload.h"

#include <iostream>
#include <spdlog/spdlog.h>

namespace stride {
namespace viewers {

/// Viewer of Simulator for cases output.
class PersonsViewer
{
public:
        /// Instantiate cases viewer.
        explicit PersonsViewer(const std::string& output_prefix) : m_persons_file(output_prefix) {}

        /// Let viewer perform update.
        void Update(const sim_event::Payload& p);

private:
        output::PersonsFile m_persons_file;
};

} // namespace viewers
} // namespace stride
