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

#include "TestResult.hpp"

namespace myhayai {

/// BoxPlotData.
struct BoxPlotData
{
public:
        BoxPlotData() : m_total(), m_min(), m_max(), m_median(), m_quartile1(), m_quartile3() {}

        typename TestResult::value_type m_total;
        typename TestResult::value_type m_min;
        typename TestResult::value_type m_max;
        typename TestResult::value_type m_median;
        typename TestResult::value_type m_quartile1;
        typename TestResult::value_type m_quartile3;

        ///
        static BoxPlotData Calculate(const TestResult& runTimes);
};

} // namespace myhayai
