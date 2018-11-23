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

#include <tclap/CmdLine.h>
#include <calibration/Calibrator.h>
#include <calibration/ConfigFileCalibrationRunner.h>
#include <calibration/TestCalibrationRunner.h>
#include <util/LogUtils.h>

using namespace calibration;
using namespace TCLAP;

int main(int argc, char* argv[])
{
        // -----------------------------------------------------------------------------------------
        // Parse command line.
        // -----------------------------------------------------------------------------------------
        std::string examples_message = "Examples:\n"
                                       "To find the exact values for the testcases and write these to a file:\n"
                                       "\tcalibration -s -o out.json\n"
                                       "To run a configuration file 10 times with a random seed and display the "
                                       "generated boxplot for the last step in the simulation:\n"
                                       "\tcalibration -c run_default.xml -m 10 -d\n"
                                       "To run the testcase `influenza_a` 10 times, write the results to a file and "
                                       "for each step in the simulation write a boxplot to a file:\n"
                                       "\tcalibration -t influenza_a -m 10 -w -o out.json\n";

        CmdLine                cmd(examples_message, ' ', "1.0");
        ValueArg<unsigned int> displayStep("D", "displayStep", "Display the boxplots for a specified step", false, 0,
                                           "step", cmd);

        SwitchArg display("d", "display", "Display the boxplots for the last step", cmd);

        SwitchArg             write("w", "write", "Write boxplots to files in the current directory", cmd);
        ValueArg<std::string> output("o", "output", "Write the results of the calibration to files", false,
                                     "results.txt", "filename", cmd);
        SwitchArg single("s", "single", "Run the simulations with the given seeds to determine the exact values", cmd);
        ValueArg<unsigned int> count("m", "multiple", "The amount of simulations to run for each testcase", false, 0,
                                     "amount", cmd);
        MultiArg<std::string> testcases("t", "testcases", "The testcases to use for the calibration", false, "testcase",
                                        cmd);
        std::string           sc = "Specifies the run configuration parameters to be used. It may be either "
                         "-c file=<file> or -c name=<name>. The first option can be shortened to -c <file>, the second "
                         "option accepts 'TestsInfluenza', 'TestsMeasles' or 'BenchMeasles' as <name>.";

        MultiArg<std::string> configArg("c", "config", sc, false, "RUN CONFIGURATION", cmd);
        cmd.parse(argc, static_cast<const char* const*>(argv));

        // Register travel logger here to avoid race conditions later
        spdlog::register_logger(stride::util::LogUtils::CreateNullLogger("travel_logger"));

        // -----------------------------------------------------------------------------------------
        // Validate provided options & setup calibrationRunner.
        // -----------------------------------------------------------------------------------------
        if (!count.isSet() && !single.isSet()) {
                std::cerr << "Please run at least one simulation." << std::endl;
                cmd.getOutput()->usage(cmd);
                return 1;
        }

        if (count.getValue() <= 1 && (write.getValue() || display.isSet())) {
                std::cerr << "Invalid parameters: cannot generate boxplots with count value " << count.getValue()
                          << std::endl;
                return 1;
        }
        if (displayStep.isSet() && display.isSet()) {
                std::cerr << "Please only select one of -d/--display and -D/--displayStep" << std::endl;
                return 1;
        }
        std::shared_ptr<CalibrationRunner> calibrationRunner;
        if (configArg.isSet()) {
                if (testcases.isSet()) {
                        std::cerr << "Cannot run the calibration with both a configuration file and a testcase "
                                     "supplied, please choose one."
                                  << std::endl;
                        cmd.getOutput()->usage(cmd);
                        return 1;
                }
                calibrationRunner = std::make_shared<ConfigFileCalibrationRunner>(configArg.getValue());
        } else {
                std::vector<std::string> tests;
                if (testcases.isSet()) {
                        tests = testcases.getValue();
                } else {
                        tests = {"influenza_a", "influenza_b", "influenza_c", "measles_16", "r0_12"};
                }
                calibrationRunner = std::make_shared<TestCalibrationRunner>(tests);
        }

        // -----------------------------------------------------------------------------------------
        // Run simulations
        // -----------------------------------------------------------------------------------------
        calibrationRunner->Run(count.getValue(), single.getValue());

        // -----------------------------------------------------------------------------------------
        // Output.
        // -----------------------------------------------------------------------------------------
        if (output.isSet())
                calibrationRunner->WriteResults(output.getValue());

#if Qt5Charts_FOUND
        if (displayStep.isSet()) {
                calibrationRunner->DisplayBoxplots(display.getValue());
        } else if (display.isSet()) {
                calibrationRunner->DisplayBoxplots();
        }
        if (write.getValue())
                calibrationRunner->WriteBoxplots();
#else
        if (display.isSet() || write.getValue())
                std::cerr << "The calibration tool was not compiled with support for Qt5Charts" << std::endl;
#endif
        return 0;
}
