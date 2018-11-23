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

#include "CalibrationRunner.h"
#include "Calibrator.h"

namespace calibration {

/**
 * A runner for the Calibration
 */
class ConfigFileCalibrationRunner : public CalibrationRunner
{
public:
        /// Setup, creates a ptree based on the filenames in configFiles
        ConfigFileCalibrationRunner(std::vector<std::string> configFiles);
};

} // namespace calibration
