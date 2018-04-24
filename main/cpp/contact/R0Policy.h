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
 * Implementation of Infector algorithms.
 */

#include "pop/Person.h"

namespace stride {

/// Primary R0_POLICY: do nothing i.e. track all cases.
/// \tparam TIC         TrackIndexCase
template <bool TIC>
class R0_POLICY
{
public:
        static void Exec(Person*) {}
};

/// Specialized R0_POLICY: track only the index case.
template <>
class R0_POLICY<true>
{
public:
        static void Exec(Person* p) { p->GetHealth().StopInfection(); }
};

} // namespace stride
