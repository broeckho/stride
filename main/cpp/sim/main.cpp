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
#include "util/RunConfigPtree.h"
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
                CmdLine          cmd("stride", ' ', "1.0", false);
                MultiArg<string> override_Arg("", "override", "config parameter override --override <name>=<value>",
                                              false, "parameter override", cmd);
                ValueArg<string> config_file_Arg("", "config", "config file", false, "run_default.xml",
                                                 "CONFIGURATION FILE", cmd);
                SwitchArg install_dir_Arg("", "working_dir", "Look for files in install directory", cmd, true);
                cmd.parse(argc, static_cast<const char* const*>(argv));

                // -----------------------------------------------------------------------------------------
                // Retrieve configuration.
                // -----------------------------------------------------------------------------------------
                const bool                    use_install_dirs = !install_dir_Arg.getValue();
                const auto                    config_fn        = config_file_Arg.getValue();
                const boost::filesystem::path config_p =
                    (use_install_dirs) ? FileSys::GetConfigDir() /= config_fn : config_fn;
                auto pt = FileSys::ReadPtreeFile(config_p);

                // -----------------------------------------------------------------------------------------
                // If configuration OK, go ahead otherwise just exit.
                // -----------------------------------------------------------------------------------------
                if (!pt.empty()) {
                        // ---------------------------------------------------------------------------------
                        // Patch configuration with the overrides (if any).
                        // ---------------------------------------------------------------------------------
                        vector<tuple<string, string>> p_overrides;
                        const auto                    p_vec = override_Arg.getValue();
                        for (const auto& p_assignment : p_vec) {
                                const auto v = util::Tokenize(p_assignment, "=");
                                pt.put("run." + v[0], v[1]);
                        }
                        auto output_prefix = pt.get<string>("run.output_prefix", "");
                        if (output_prefix.empty()) {
                                output_prefix = TimeStamp().ToTag() + "/";
                                pt.put("run.output_prefix", output_prefix);
                        }
                        pt.sort();

                        // ---------------------------------------------------------------------------------
                        // Setup controller, run simulation.
                        // ---------------------------------------------------------------------------------
                        CliController cntrl(pt);
                        cntrl.Setup();
                        cntrl.Execute();
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
