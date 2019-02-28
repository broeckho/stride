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

#include "TimeStamp.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace stride {
namespace util {

TimeStamp::TimeStamp() : m_tp(std::chrono::system_clock::now()) {}

std::string TimeStamp::ToString() const
{
        std::time_t t   = std::chrono::system_clock::to_time_t(m_tp);
        std::string str = std::ctime(&t);
        // str[str.length() - 1] = ' ';
        return str.substr(0, str.length() - 1);
}

std::string TimeStamp::ToTag() const
{
        // This is the C++11 implementation but clang 5 on Travis Linux VM's
        // still does not implement std::put_time.
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%X");
        return ss.str();
        /*
        time_t    now = time(nullptr);
        struct tm tstruct{};
        char      buf[80];
        tstruct = *localtime_r(&now, &tstruct);
        strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
        return std::string(buf);
         */
}

} // namespace util
} // namespace stride
