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

#include "myhayai/BoxPlotData.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace std;

namespace myhayai {

BoxPlotData BoxPlotData::Calculate(const vector<uint64_t>& runTimes)
{
        const auto size = runTimes.size();
        assert(size >= 1 && "TestResult> cannot calculate stats for empty vector.");

        // Total, min, max
        BoxPlotData stats;
        stats.m_total   = accumulate(runTimes.cbegin(), runTimes.cend(), static_cast<uint64_t>(0));
        stats.m_max     = *max_element(runTimes.cbegin(), runTimes.cend());
        stats.m_min     = *min_element(runTimes.cbegin(), runTimes.cend());
        stats.m_average = static_cast<double>(stats.m_total) / static_cast<double>(runTimes.size());

        // Standard deviation (or 0 if size == 1).
        const double dev = accumulate(runTimes.cbegin(), runTimes.cend(), static_cast<uint64_t>(0));
        stats.m_std_dev  = (size > 1) ? sqrt(dev / (size - 1)) : 0.0;

        // Quartiles.
        std::vector<uint64_t> sTimes(runTimes);
        std::sort(sTimes.begin(), sTimes.end());
        const std::size_t sizeHalf = size / 2;
        if (size >= 4) {
                const std::size_t quartile = sizeHalf / 2;
                if ((size % 2) == 0) {
                        stats.m_median    = 0.5 * (double(sTimes[sizeHalf - 1]) + double(sTimes[sizeHalf]));
                        stats.m_quartile1 = double(sTimes[quartile]);
                        stats.m_quartile3 = double(sTimes[sizeHalf + quartile]);
                } else {
                        stats.m_median    = double(sTimes[sizeHalf]);
                        stats.m_quartile1 = 0.5 * (double(sTimes[quartile - 1]) + double(sTimes[quartile]));
                        stats.m_quartile3 =
                            0.5 * (double(sTimes[sizeHalf + (quartile - 1)]) + double(sTimes[sizeHalf + quartile]));
                }
        } else if (size > 0) {
                stats.m_median    = double(sTimes[sizeHalf]);
                stats.m_quartile1 = double(sTimes.front());
                stats.m_quartile3 = double(sTimes.back());
        }

        return stats;
}

} // namespace myhayai
