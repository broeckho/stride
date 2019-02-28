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
 * TimeStamp class.
 */

#pragma once

#include <chrono>
#include <ostream>
#include <string>

namespace stride {
namespace util {

/**
 * Provides wall-clock time stamp using the time call.
 * The time is that of the constructor call.
 */
class TimeStamp
{
public:
        /// Constructor marks the current time for the time stamp.
        TimeStamp();

        /// Returns string with the time stamp after eliminating newline.
        std::string ToString() const;

        /// Returns string with the time stamp after eliminating newline.
        std::string ToTag() const;

private:
        std::chrono::system_clock::time_point m_tp;
};

/**
 * TimeStamp helper inserts string representation in output stream.
 */
inline std::ostream& operator<<(std::ostream& os, TimeStamp t) { return (os << t.ToString()); }

} // namespace util
} // namespace stride
