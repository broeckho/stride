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

#include "CalibrationRunner.h"
#include "../../test/cpp/gtester/ScenarioData.h"
#include "Boxplot.h"
#include "BoxplotGenerator.h"
#include "Calibrator.h"

#ifdef QTCHARTS
#include "BoxplotGenerator.h"
#endif

namespace calibration {

CalibrationRunner::CalibrationRunner() : configs(), multipleResults(), singleResults(), calibrator() {}

void CalibrationRunner::Run(unsigned int count, bool single)
{
        Calibrator calibrator;

        if (single) {
                singleResults = calibrator.RunSingle(configs);
                calibrator.PrintSingleResults(singleResults);
        }
        if (count > 0) {
                multipleResults = calibrator.RunMultiple(count, configs);
                calibrator.PrintMultipleResults(multipleResults);
        }
}

void CalibrationRunner::WriteResults(std::string filename)
{
        if (singleResults.size() && multipleResults.size())
                calibrator.WriteResults(singleResults, multipleResults, filename);
        else if (singleResults.size())
                calibrator.WriteSingleResults(singleResults, filename);
        else if (multipleResults.size())
                calibrator.WriteMultipleResults(multipleResults, filename);
}

#if Qt5Charts_FOUND == true
void CalibrationRunner::WriteBoxplots()
{
        Boxplot                  boxplot;
        std::vector<BoxplotData> data = boxplot.Calculate(multipleResults);
        BoxplotGenerator         boxplotGenerator;
        boxplotGenerator.WriteToFile(data);
}

void CalibrationRunner::DisplayBoxplots(unsigned int step)
{
        Boxplot                  boxplot;
        std::vector<BoxplotData> data = boxplot.Calculate(multipleResults, step);
        BoxplotGenerator         boxplotGenerator;
        boxplotGenerator.Display(data);
}

void CalibrationRunner::DisplayBoxplots()
{
        Boxplot                  boxplot;
        std::vector<BoxplotData> data = boxplot.CalculateLastStep(multipleResults);
        BoxplotGenerator         boxplotGenerator;
        boxplotGenerator.Display(data);
}

#endif

} // namespace calibration
