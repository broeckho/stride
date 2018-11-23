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
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

#include "Calibrator.h"

namespace calibration {

/**
 * A runner for the Calibration
 */
class CalibrationRunner
{
public:
        /// Setup
        CalibrationRunner();

        /// Run the Calibrator
        void Run(unsigned int count, bool single);

        /// Write the results to their respective files (based on testcase name)
        void WriteResults(std::string filename);

        /// Write the boxplots to their respective files (based on testcase name)
        void WriteBoxplots();

        /// Display the generated boxplots for step 'step' in the simulation to the screen
        void DisplayBoxplots(unsigned int step);

        /// Display the generated boxplots for the last step in the simulation to the screen
        void DisplayBoxplots();

protected:
        std::vector<std::pair<boost::property_tree::ptree, std::string>> configs; ///< The configs for the testcases
        std::map<std::string, std::vector<std::vector<unsigned int>>>    multipleResults; ///< Results from RunMultiple
        std::map<std::string, std::vector<unsigned int>>                 singleResults;   ///< Results from RunSingle
        Calibrator calibrator; ///< The Calibrator to use for calibrating the testcases
};

} // namespace calibration
