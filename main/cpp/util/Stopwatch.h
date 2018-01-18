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
 * Definition of Stopwatch.
 */

#include "TimeToString.h"
#include <chrono>
#include <string>

namespace stride {
namespace util {

/**
 * Provides a stopwatch interface to time: it accumulates time between
 * start/stop pairs.
 */
template <typename T = std::chrono::system_clock>
class Stopwatch
{
public:
        typedef T TClock;

        /// Constructor initializes stopwatch.
        Stopwatch(std::string name = "stopwatch", bool running = false)
            : m_accumulated(T::duration::zero()), m_name(name), m_running(running)
        {
                if (m_running) {
                        m_last_start = T::now();
                }
        }

        /// Starts stopwatch if it was stopped.
        Stopwatch& Start()
        {
                if (!m_running) {
                        m_running = true;
                        m_last_start = T::now();
                }
                return *this;
        }

        /// Stops the stopwatch if it was running.
        Stopwatch& Stop()
        {
                if (m_running) {
                        m_accumulated += (T::now() - m_last_start);
                        m_running = false;
                }
                return *this;
        }

        /// Resets stopwatch i.e. stopwatch is stopped and time accumulator is
        /// cleared.
        Stopwatch& Reset()
        {
                m_accumulated = T::duration::zero();
                m_running = false;
                return *this;
        }

        /// Reports whether stopwatch has been started.
        bool IsRunning() const { return (m_running); }

        /// Return name of this stopwatch
        std::string GetName() const { return m_name; }

        /// Returns the accumulated value without altering the stopwatch state.
        typename T::duration Get() const
        {
                auto fTemp = m_accumulated;
                if (m_running) {
                        fTemp += (T::now() - m_last_start);
                }
                return fTemp;
        }

        /// Returns string representation of readout
        std::string ToString() const
        {
                using namespace std;
                using namespace std::chrono;

                string colon_string;
                typedef typename TClock::period TPeriod;
                if (ratio_less_equal<TPeriod, micro>::value) {
                        microseconds d = duration_cast<microseconds>(Get());
                        colon_string = TimeToString::ToColonString(d);
                } else if (ratio_less_equal<TPeriod, milli>::value) {
                        milliseconds d = duration_cast<milliseconds>(Get());
                        colon_string = TimeToString::ToColonString(d);
                } else {
                        seconds d = duration_cast<seconds>(Get());
                        colon_string = TimeToString::ToColonString(d);
                }
                return colon_string;
        }

private:
        typename T::duration m_accumulated;
        typename T::time_point m_last_start;
        std::string m_name;
        bool m_running;
};

/**
 * Insert accumulated time into output stream without altering stopwatch state.
 */
template <typename T>
std::ostream& operator<<(std::ostream& oss, Stopwatch<T> const& stopwatch)
{
        return (oss << stopwatch.ToString());
}

} // namespace util
} // namespace stride
