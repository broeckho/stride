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
 */
/**
 * @file
 * Header file for BoxPlotData.
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

namespace stride {
namespace util {

/// BoxPlotData calculates total, minimum, maximum, median, lower and upper
/// quartile of the values in the vector.
/// \tparam T   Datatype for which boxplot data get calculated.
template <typename T>
struct BoxPlotData
{
public:
        BoxPlotData() : m_total(), m_min(), m_max(), m_median(), m_quartile1(), m_quartile3() {}

        T m_total;
        T m_min;
        T m_max;
        T m_median;
        T m_quartile1;
        T m_quartile3;

        ///
        static BoxPlotData Calculate(const std::vector<T>& data)
        {
                assert(data.size() >= 1 && "TestResult> cannot calculate stats for empty vector.");
                BoxPlotData stats;

                // Sort the durations.
                auto temp(data);
                std::sort(temp.begin(), temp.end());

                // Total, min, max
                stats.m_total = std::accumulate(temp.cbegin(), temp.cend(), T());
                stats.m_min   = temp.front();
                stats.m_max   = temp.back();

                // Median, quartiles.
                const std::size_t size     = temp.size();
                const std::size_t sizeHalf = size / 2;
                if (size >= 4) {
                        const std::size_t quartile = sizeHalf / 2;
                        if ((size % 2) == 0) {
                                stats.m_median    = (temp[sizeHalf - 1] + temp[sizeHalf]) / 2;
                                stats.m_quartile1 = temp[quartile];
                                stats.m_quartile3 = temp[sizeHalf + quartile];
                        } else {
                                stats.m_median    = temp[sizeHalf];
                                stats.m_quartile1 = (temp[quartile - 1] + temp[quartile]) / 2;
                                stats.m_quartile3 = (temp[sizeHalf + (quartile - 1)] + temp[sizeHalf + quartile]) / 2;
                        }
                } else if (size > 0) {
                        stats.m_median    = temp[sizeHalf];
                        stats.m_quartile1 = temp.front();
                        stats.m_quartile3 = temp.back();
                }

                return stats;
        }
};

} // namespace util
} // namespace stride
