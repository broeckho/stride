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
#include "util/FileSys.h"
#include "util/RunConfigManager.h"
#include "util/StringUtils.h"
#include "util/TimeStamp.h"

#include <tclap/CmdLine.h>
#include <iomanip>
#include <ios>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace TCLAP;
using namespace boost::property_tree;

/// Main program of the stride simulator.
int main(int argc, char** argv)
{
        int exit_status = EXIT_SUCCESS;

        try {
                // -----------------------------------------------------------------------------------------
                // Parse command line.
                // -----------------------------------------------------------------------------------------
                CmdLine cmd("stride", ' ', "1.0");

                vector<string>           execs{"clean_config", "sim"};
                ValuesConstraint<string> vc(execs);
                ValueArg<string>         exec("", "exec", "Execute the specified function.", false, "sim", &vc, cmd);
                MultiArg<string>         override_Arg("", "override",
                                              "Override config parameters. Format --override <name>=<value>", false,
                                              "<NAME>=<VALUE>", cmd);
                ValueArg<string> config_file_Arg("", "config", "File with the run configuration parameters.", false,
                                                 "run_default.xml", "CONFIGURATION FILE", cmd);
                SwitchArg installed_config_Arg("", "installed_config", "Look for config file in install directories",
                                               cmd, true);
                cmd.parse(argc, static_cast<const char* const*>(argv));

                // -----------------------------------------------------------------------------------------
                // Full configuration filename.
                // -----------------------------------------------------------------------------------------
                const auto                    config_fn = config_file_Arg.getValue();
                const boost::filesystem::path config_fp =
                    (installed_config_Arg.getValue()) ? FileSys::GetConfigDir() /= config_fn : config_fn;

                // -----------------------------------------------------------------------------------------
                // If run simulation ...
                // -----------------------------------------------------------------------------------------
                if (exec.getValue() == "sim") {

                        // Read configuration and patch it with the overrides (if any).
                        auto pt = FileSys::ReadPtreeFile(config_fp);
                        for (const auto& p_assignment : override_Arg.getValue()) {
                                const auto v = util::Tokenize(p_assignment, "=");
                                pt.put("run." + v[0], v[1]);
                        }
                        if (pt.get<string>("run.output_prefix", "").empty()) {
                                pt.put("run.output_prefix", TimeStamp().ToTag().append("/"));
                        }
                        pt.sort();

                        // Setup controller, run simulation.
                        CliController cntrl(pt);
                        cntrl.Setup();
                        cntrl.Execute();
                }
                // -----------------------------------------------------------------------------------------
                // If clean an configuration file
                // -----------------------------------------------------------------------------------------
                else if (exec.getValue() == "clean_config") {
                        RunConfigManager::CleanConfigFile(config_fp);
                }

        } catch (exception& e) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }
        return exit_status;
}
