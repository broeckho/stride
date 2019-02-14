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
 * Parameters (mostly age-related) for occupational (school, work, ...) categories.
 * These categories map into types of ContactPool in the simulator algorithm.
 * Some of the parameters (non-constexpr) will be overwritten by data from config file.
 */
namespace PoolConfig {

        /// DayCare with lower (inclusive) and upper (exclusive) age range
        /// and fraction of age bracket that is in DayCare.
        struct DayCare
        {
                constexpr static inline double m_lower = 0.0;  // if changed, activate test in HasAge!
                constexpr static inline double m_upper = 3.0;
                static inline double           m_fraction = 0.5;

                static bool HasAge(double age) {return /*age >= m_lower && */ age < m_upper;}
        };

        /// Preschool with lower (inclusive) and upper (exclusive) age range
        /// and fraction of age bracket that attends PreSchool.
        struct PreSchool
        {
                constexpr static inline double m_lower = 3.0;
                constexpr static inline double m_upper = 6.0;
                static inline double           m_fraction = 0.9;

                static bool HasAge(double age) {return age >= m_lower && age < m_upper;}
        };

        /// K12school with lower (inclusive) and upper (exclusive) age range
        /// and fraction of age bracket that attends K12School.
        struct K12School
        {
                constexpr static inline double m_lower = 6.0;
                constexpr static inline double m_upper = 18.0;
                constexpr static inline double m_fraction = 1.0;   // So, no home schooling.

                static bool HasAge(double age) {return age >= m_lower && age < m_upper;}
        };


        /// College with lower (inclusive) and upper (exclusive) age range
        /// and fraction of age bracket that attends College.
        struct College
        {
                constexpr static inline double m_lower = 18.0;
                constexpr static inline double m_upper = 26.0;
                static inline double           m_fraction = 0.5;

                static bool HasAge(double age) {return age >= m_lower && age < m_upper;}
        };


        /// College with lower (inclusive) and upper (exclusive) age range
        /// and fraction of age bracket that is employed.
        struct Workplace
        {
                constexpr static inline double m_lower = 18U;
                constexpr static inline double m_upper = 65u;
                static inline double           m_fraction = 0.75;

                static bool HasAge(double age) {return age >= m_lower && age < m_upper;}
        };

} // namespace PoolConfig
} // namespace stride
