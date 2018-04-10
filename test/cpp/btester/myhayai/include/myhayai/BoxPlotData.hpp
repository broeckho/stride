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
 * Header file for BoxPlotData.
 */

#include <cstdint>
#include <vector>

namespace myhayai {

/// BoxPlotData.
struct BoxPlotData
{
public:
        BoxPlotData() : m_total(), m_min(), m_max(), m_average(), m_median(), m_std_dev(), m_quartile1(), m_quartile3()
        {
        }

        uint64_t m_total;
        uint64_t m_min;
        uint64_t m_max;
        double   m_average;
        double   m_median;
        double   m_std_dev;
        double   m_quartile1;
        double   m_quartile3;

        ///
        static BoxPlotData Calculate(const std::vector<uint64_t>& runTimes);
};

} // namespace myhayai
