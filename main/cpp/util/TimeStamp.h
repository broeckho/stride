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
 * TimeStamp class.
 */

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
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
        /// Constructor marks the time for the time stamp.
        TimeStamp() : m_tp(std::chrono::system_clock::now()) {}

        /// Returns string with the time stamp after eliminating newline.
        std::string ToString() const
        {
                std::time_t t   = std::chrono::system_clock::to_time_t(m_tp);
                std::string str = std::ctime(&t);
                // str[str.length() - 1] = ' ';
                return str.substr(0, str.length() - 1);
        }

        /// Returns string with the time stamp after eliminating newline.
        std::string ToTag() const
        {
                // This is the C++11 implementation but gcc (at least up to 4.9)
                // does not implement std::put_time.
                // auto now = std::chrono::system_clock::now();
                // auto in_time_t = std::chrono::system_clock::to_time_t(now);
                // std::stringstream ss;
                // ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%X");
                // return ss.str();

                time_t    now = time(NULL);
                struct tm tstruct;
                char      buf[80];
                tstruct = *localtime(&now);
                strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
                return buf;
        }

        /// Returns time stamp as a time_t.
        std::time_t ToTimeT() const { return std::chrono::system_clock::to_time_t(m_tp); }

private:
        std::chrono::system_clock::time_point m_tp;
};

/**
 * TimeStamp helper inserts string representation in output stream.
 */
inline std::ostream& operator<<(std::ostream& os, TimeStamp t) { return (os << t.ToString()); }

} // namespace util
} // namespace stride
