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
 * Header for the SimulatorObserver class.
 */

namespace stride {
namespace python {

/// Hook to allow python environment to get simulator state.
class SimulatorObserver
{
public:
        SimulatorObserver() {}

        virtual ~SimulatorObserver() {}

        /// Update with time step as an argument.
        virtual void Update(unsigned int) {}
};

} // namespace python
} // namespace stride
