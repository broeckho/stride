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
 * @file Header for NoBehaviour policy
 */

namespace stride {

/**
 * NoBehaviour means that whatever beliefs a person holds, or whatever
 * information they gather, will not result in a change in their behaviour.
 */
template <typename belief_policy>
class NoBehaviour
{
public:
        static bool PracticesSocialDistancing(const typename belief_policy::Data& belief_data) { return false; }

        static bool PracticesVaccination(const typename belief_policy::Data& belief_data) { return false; }
};

} // namespace stride
