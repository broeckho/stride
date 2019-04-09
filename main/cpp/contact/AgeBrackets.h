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
 *  Copyright 2019, Jan Broeckhove.
 */

namespace stride {

/**
 * AgeBrackets that determine participation in type of ContactPool.
 */
namespace AgeBrackets {

/// DayCare with lower (inclusive) and upper (exclusive) age bracket..
struct Daycare
{
        constexpr static inline double m_lower = 0.0; // if changed, activate test in HasAge!
        constexpr static inline double m_upper = 3.0;

        static bool HasAge(double age) { return /*age >= m_lower && */ age < m_upper; }
};

/// Preschool with lower (inclusive) and upper (exclusive) age bracket.
struct PreSchool
{
        constexpr static inline double m_lower = 3.0;
        constexpr static inline double m_upper = 6.0;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

/// K12school with lower (inclusive) and upper (exclusive) age bracket.
struct K12School
{
        constexpr static inline double m_lower = 6.0;
        constexpr static inline double m_upper = 18.0;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

/// College with lower (inclusive) and upper (exclusive) age bracket.
struct College
{
        constexpr static inline double m_lower = 18.0;
        constexpr static inline double m_upper = 26.0;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

/// College with lower (inclusive) and upper (exclusive) age bracket.
struct Workplace
{
        constexpr static inline double m_lower = 18U;
        constexpr static inline double m_upper = 65u;

        static bool HasAge(double age) { return age >= m_lower && age < m_upper; }
};

} // namespace AgeBrackets
} // namespace stride
