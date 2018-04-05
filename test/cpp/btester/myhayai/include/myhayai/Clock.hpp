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
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */

/**
 * @file
 * Header file for Clock.
 */

//
// System-specific implementation of the clock functions.
//
// Copyright (C) 2011 Nick Bruun <nick@bruun.co>
// Copyright (C) 2013 Vlad Lazarenko <vlad@lazarenko.me>
// Copyright (C) 2014 Nicolas Pauss <nicolas.pauss@gmail.com>
//
// Implementation notes:
//
// On Windows, QueryPerformanceCounter() is used. It gets
// real-time clock with up to nanosecond precision.
//
// On Apple (OS X, iOS), mach_absolute_time() is used. It gets
// CPU/bus dependent real-time clock with up to nanosecond precision.
//
// On Unix clock_gettime() is used to access monotonic real-time clock
// with up to nanosecond precision. On kernels 2.6.28 and newer, the ticks
// are also raw and are not subject to NTP and/or adjtime(3) adjustments.
//
// Note on C++11:
//
// Starting with C++11, we could use std::chrono. However, the details of
// what clock is really being used is implementation-specific. For example,
// Visual Studio 2012 defines "high_resolution_clock" as system clock with
// ~1 millisecond precision that is not acceptable for performance
// measurements. Therefore, we are much better off having full control of what
// mechanism we use to obtain the system clock.
//
// Note on durations: it is assumed that end times passed to the clock methods
// are all after the start time. Wrap-around of clocks is not tested, as
// nanosecond precision of unsigned 64-bit integers would require an uptime of
// almost 585 years for this to happen. Let's call ourselves safe on that one.
//

// Apple
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>

#elif defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
#include <time.h>

#else
#error "Unable to define high resolution timer for an unknown OS."
#endif

#include <cstdint>
#include <stdexcept>

namespace myhayai {

// Mach kernel.
#if defined(__APPLE__) && defined(__MACH__)
class Clock
{
public:
        /// Time point.
        /// Opaque representation of a point in time.
        using TimePoint = uint64_t;

        /// Get the current time as a time point.
        /// @returns the current time point.
        static TimePoint Now() noexcept { return mach_absolute_time(); }

        /// Get the duration between two time points.
        /// @param startTime Start time point.
        /// @param endTime End time point.
        /// @returns the number of nanoseconds elapsed between the two time points.
        static uint64_t Duration(const TimePoint& startTime, const TimePoint& endTime) noexcept
        {
                mach_timebase_info_data_t time_info;
                mach_timebase_info(&time_info);
                return (endTime - startTime) * time_info.numer / time_info.denom;
        }

        /// Clock implementation description.
        /// @returns a description of the clock implementation used.
        static const char* Description() { return "mach_absolute_time"; }
};

// clock_gettime
#elif defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
class Clock
{
public:
        /// Time point.

        /// Opaque representation of a point in time.
        typedef struct timespec TimePoint;

        /// Get the current time as a time point.

        /// @returns the current time point.
        static TimePoint Now() __hayai_noexcept
        {
                TimePoint result;
#if defined(CLOCK_MONOTONIC_RAW)
                clock_gettime(CLOCK_MONOTONIC_RAW, &result);
#elif defined(CLOCK_MONOTONIC)
                clock_gettime(CLOCK_MONOTONIC, &result);
#elif defined(CLOCK_REALTIME)
                clock_gettime(CLOCK_REALTIME, &result);
#else
                clock_gettime((clockid_t)-1, &result);
#endif
                return result;
        }

        /// Get the duration between two time points.

        /// @param startTime Start time point.
        /// @param endTime End time point.
        /// @returns the number of nanoseconds elapsed between the two time
        /// points.
        static uint64_t Duration(const TimePoint& startTime, const TimePoint& endTime) __hayai_noexcept
        {
                TimePoint timeDiff;

                timeDiff.tv_sec = endTime.tv_sec - startTime.tv_sec;
                if (endTime.tv_nsec < startTime.tv_nsec) {
                        timeDiff.tv_nsec = endTime.tv_nsec + 1000000000LL - startTime.tv_nsec;
                        timeDiff.tv_sec--;
                } else
                        timeDiff.tv_nsec = endTime.tv_nsec - startTime.tv_nsec;

                return static_cast<uint64_t>(timeDiff.tv_sec * 1000000000LL + timeDiff.tv_nsec);
        }

        /// Clock implementation description.

        /// @returns a description of the clock implementation used.
        static const char* Description()
        {
#if defined(CLOCK_MONOTONIC_RAW)
                return "clock_gettime(CLOCK_MONOTONIC_RAW)";
#elif defined(CLOCK_MONOTONIC)
                return "clock_gettime(CLOCK_MONOTONIC)";
#elif defined(CLOCK_REALTIME)
                return "clock_gettime(CLOCK_REALTIME)";
#else
                return "clock_gettime(-1)";
#endif
        }
};

#endif

} // namespace myhayai
