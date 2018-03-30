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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Main program: command line handling.
 */

#include "sim/CliController.h"
#include "util/StringUtils.h"

#include <tclap/CmdLine.h>
#include <vector>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace TCLAP;

/// Main program of the stride simulator.
int main(int argc, char** argv)
{
        int exit_status = EXIT_SUCCESS;

        try {
                // -----------------------------------------------------------------------------------------
                // Parse command line.
                // -----------------------------------------------------------------------------------------
                CmdLine cmd("stride", ' ', "1.0", false);

                vector<string>           vals{"trace", "debug", "info", "warning", "error", "critical"};
                ValuesConstraint<string> vc(vals);
                ValueArg<string>         stride_log_level("l", "loglevel", "stride log level", false, "info", &vc, cmd);
                MultiArg<string>         params_override_Arg("p", "param_override",
                                                     "config parameter override -p <p_name>=<p_value>", false,
                                                     "parameter override", cmd);
                ValueArg<string>         config_file_Arg("c", "config", "config file", false, "run_default.xml",
                                                 "CONFIGURATION FILE", cmd);

                SwitchArg index_case_Arg("r", "r0", "R0 only i.e. track index case mode", cmd, false);
                SwitchArg working_dir_Arg("w", "working_dir",
                                          "Use working directory to find files i.o install directories", cmd, false);

                cmd.parse(argc, static_cast<const char* const*>(argv));

                // -----------------------------------------------------------------------------------------
                // Parse commandline config parameter overrides (if any).
                // -----------------------------------------------------------------------------------------
                vector<tuple<string, string>> p_overrides;
                const auto                    p_vec = params_override_Arg.getValue();
                for (const auto& p_assignment : p_vec) {
                        const auto v = util::Tokenize(p_assignment, "=");
                        p_overrides.emplace_back(make_tuple(v[0], v[1]));
                }

                // -----------------------------------------------------------------------------------------
                // Run the Stride simulator.
                // -----------------------------------------------------------------------------------------
                // We have been using use_installdirs for a while, so ..
                const bool    use_install_dirs = !working_dir_Arg.getValue();
                CliController cntrl(config_file_Arg.getValue(), p_overrides, index_case_Arg.getValue(),
                                    stride_log_level.getValue(), use_install_dirs);
                cntrl.Setup();
                cntrl.Execute();
        } catch (exception& e) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }
        return exit_status;
}
