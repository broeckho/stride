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
 * Header file for TestResult.
 */

#include <cstdint>
#include <vector>

namespace myhayai {

/// Test result descriptor. All durations are expressed in nanoseconds.
struct TestResult
{
public:
        /// Initialize test result descriptor.
        /// @param runTimes Timing for the individual runs.
        /// @param iterations Number of iterations per run.
        explicit TestResult(const std::vector<std::uint64_t>& runTimes);

        /// Total time.
        inline double TimeTotal() const { return double(_timeTotal); }

        /// Run times.
        inline const std::vector<uint64_t>& RunTimes() const { return _runTimes; }

        /// Average time per run.
        inline double RunTimeAverage() const { return double(_timeTotal) / double(_runTimes.size()); }

        /// Standard deviation time per run.
        inline double RunTimeStdDev() const { return _timeStdDev; }

        /// Median (2nd Quartile) time per run.
        inline double RunTimeMedian() const { return _timeMedian; }

        /// 1st Quartile time per run.
        inline double RunTimeQuartile1() const { return _timeQuartile1; }

        /// 3rd Quartile time per run.
        inline double RunTimeQuartile3() const { return _timeQuartile3; }

        /// Maximum time per run.
        inline double RunTimeMaximum() const { return double(_timeRunMax); }

        /// Minimum time per run.
        inline double RunTimeMinimum() const { return double(_timeRunMin); }

private:
        std::vector<uint64_t> _runTimes;
        uint64_t              _timeTotal;
        uint64_t              _timeRunMin;
        uint64_t              _timeRunMax;
        double                _timeStdDev;
        double                _timeMedian;
        double                _timeQuartile1;
        double                _timeQuartile3;
};

} // namespace myhayai
