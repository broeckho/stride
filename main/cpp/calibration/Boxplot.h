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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#pragma once
#include <map>
#include <string>
#include <util/LogUtils.h>
#include <vector>

#include "BoxplotData.h"

namespace calibration {

/**
 * A class used to compute the necessary information to generate boxplots
 */
class Boxplot
{
public:
        /// Setup
        Boxplot();

        /// Compute the information for each step in a list of testcase results
        std::vector<BoxplotData> Calculate(
            const std::map<std::string, std::vector<std::vector<unsigned int>>>& data) const;

        /// Compute the information for a specific step in a list of testcase results
        std::vector<BoxplotData> Calculate(const std::map<std::string, std::vector<std::vector<unsigned int>>>& data,
                                           unsigned int step) const;

        /// Compute the information for the last step in a list of testcase results
        std::vector<BoxplotData> CalculateLastStep(
            const std::map<std::string, std::vector<std::vector<unsigned int>>>& data) const;

        /// Compute the information for a single set of data
        BoxplotData Calculate(std::vector<unsigned int> data, std::string name) const;

private:
        /// Find the median of values between begin and end in the results. This vector needs to be sorted.
        double FindMedian(unsigned long begin, unsigned long end, std::vector<unsigned int>& results) const;

        std::shared_ptr<spdlog::logger> m_logger; ///< Logger to use for this class
};

} // namespace calibration
