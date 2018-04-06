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
 *  The original copyright, to be found in the directory one level higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for TestResult.
 */

#include "myhayai/TestResult.hpp"

#include "myhayai/Clock.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

namespace myhayai {

TestResult::TestResult(const std::vector<uint64_t>& runTimes)
    : _runTimes(runTimes), _timeTotal(0), _timeRunMin(std::numeric_limits<uint64_t>::max()),
      _timeRunMax(std::numeric_limits<uint64_t>::min()), _timeStdDev(0.0), _timeMedian(0.0), _timeQuartile1(0.0),
      _timeQuartile3(0.0)
{
        // Summarize under the assumption of values being accessed more than once.
        auto runIt = _runTimes.begin();
        while (runIt != _runTimes.end()) {
                const uint64_t run = *runIt;
                _timeTotal += run;
                if ((runIt == _runTimes.begin()) || (run > _timeRunMax))
                        _timeRunMax = run;
                if ((runIt == _runTimes.begin()) || (run < _timeRunMin))
                        _timeRunMin = run;
                ++runIt;
        }

        // Calculate standard deviation.
        const double mean = RunTimeAverage();
        double       accu = 0.0;
        runIt             = _runTimes.begin();
        while (runIt != _runTimes.end()) {
                const uint64_t run  = *runIt;
                const double   diff = double(run) - mean;
                accu += (diff * diff);
                ++runIt;
        }
        _timeStdDev = std::sqrt(accu / (_runTimes.size() - 1));

        // Calculate quartiles.
        std::vector<uint64_t> sRunTimes(_runTimes);
        std::sort(sRunTimes.begin(), sRunTimes.end());
        const std::size_t sSize     = sRunTimes.size();
        const std::size_t sSizeHalf = sSize / 2;
        if (sSize >= 4) {
                const std::size_t quartile = sSizeHalf / 2;
                if ((sSize % 2) == 0) {
                        _timeMedian    = (double(sRunTimes[sSizeHalf - 1]) + double(sRunTimes[sSizeHalf])) / 2;
                        _timeQuartile1 = double(sRunTimes[quartile]);
                        _timeQuartile3 = double(sRunTimes[sSizeHalf + quartile]);
                } else {
                        _timeMedian    = double(sRunTimes[sSizeHalf]);
                        _timeQuartile1 = (double(sRunTimes[quartile - 1]) + double(sRunTimes[quartile])) / 2;
                        _timeQuartile3 =
                            (double(sRunTimes[sSizeHalf + (quartile - 1)]) + double(sRunTimes[sSizeHalf + quartile])) /
                            2;
                }
        } else if (sSize > 0) {
                _timeMedian    = double(sRunTimes[sSizeHalf]);
                _timeQuartile1 = double(sRunTimes.front());
                _timeQuartile3 = double(sRunTimes.back());
        }
}

} // namespace myhayai
