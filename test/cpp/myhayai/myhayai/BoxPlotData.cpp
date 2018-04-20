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
 */
/**
 * @file
 * Implementation file for TestResult.
 */

#include "BoxPlotData.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <numeric>
#include <vector>

using namespace std;
using namespace std::chrono;

namespace myhayai {

BoxPlotData BoxPlotData::Calculate(const TestResult& runTimes)
{
        assert(runTimes.size() >= 1 && "TestResult> cannot calculate stats for empty vector.");

        // Sort the durations.
        TestResult sTimes(runTimes);
        std::sort(sTimes.begin(), sTimes.end());

        // Total, min, max
        BoxPlotData stats;
        stats.m_total = accumulate(runTimes.cbegin(), runTimes.cend(), steady_clock::duration::zero());
        stats.m_max   = sTimes.back();
        stats.m_min   = sTimes.front();

        // Quartiles.
        const std::size_t size     = sTimes.size();
        const std::size_t sizeHalf = size / 2;
        if (size >= 4) {
                const std::size_t quartile = sizeHalf / 2;
                if ((size % 2) == 0) {
                        stats.m_median    = (sTimes[sizeHalf - 1] + sTimes[sizeHalf]) / 2;
                        stats.m_quartile1 = sTimes[quartile];
                        stats.m_quartile3 = sTimes[sizeHalf + quartile];
                } else {
                        stats.m_median    = sTimes[sizeHalf];
                        stats.m_quartile1 = (sTimes[quartile - 1] + sTimes[quartile]) / 2;
                        stats.m_quartile3 = (sTimes[sizeHalf + (quartile - 1)] + sTimes[sizeHalf + quartile]) / 2;
                }
        } else if (size > 0) {
                stats.m_median    = sTimes[sizeHalf];
                stats.m_quartile1 = sTimes.front();
                stats.m_quartile3 = sTimes.back();
        }

        return stats;
}

} // namespace myhayai
