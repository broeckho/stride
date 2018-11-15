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

#include "TestCalibrationRunner.h"
#include "../../test/cpp/gtester/ScenarioData.h"
#include "Boxplot.h"
#include "Calibrator.h"

namespace calibration {

TestCalibrationRunner::TestCalibrationRunner(std::vector<std::string> testcases) : CalibrationRunner()
{
        for (const auto& testcase : testcases) {
                const auto  d         = Tests::ScenarioData::Get(testcase);
                const auto& config_pt = std::get<0>(d);
                configs.push_back(std::make_pair(config_pt, testcase));
        }
}

} // namespace calibration
