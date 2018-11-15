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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Main program: command line handling.
 */

#include "sim/CliController.h"
#include "sim/StanController.h"
#include "util/FileSys.h"
#include "util/RunConfigManager.h"
#include "util/StringUtils.h"
#include "util/TimeStamp.h"

#include <tclap/CmdLine.h>
#include <iostream>
#include <regex>
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
        int exitStatus = EXIT_SUCCESS;

        try {
                // -----------------------------------------------------------------------------------------
                // Parse command line (parameters displayed in --help in reverse order to order below).
                // -----------------------------------------------------------------------------------------
                CmdLine cmd("stride", ' ', "1.0");

                string sa = "Stochastic Analysis (stan) will run <COUNT> simulations, each with "
                            "a different seed for the random engine."
                            "This option only applies in case of -e sim. It effects overrides to the "
                            "configuration. See the manual for more info.";
                ValueArg<unsigned int> stanArg("", "stan", sa, false, 0, "COUNT", cmd);

                string si = "Look for configuration file specified by the -c file=<file> or -c <file> in the "
                            "stride install directories";
                SwitchArg installedArg("i", "installed", si, cmd, true);

                string           so = "Override parameters in the configuration specified with --config.";
                MultiArg<string> overrideArg("o", "override", so, false, "<NAME>=<VALUE>", cmd);

                string sc = "Specifies the run configuration parameters. The format may be either "
                            "-c file=<file> or -c name=<name>. The first is most commonly used and may be "
                            "shortened to -c <file>. The second format "
                            "refers to built-in configurations specified by their name.";
                ValueArg<string> configArg("c", "config", sc, false, "run_default.xml", "CONFIGURATION", cmd);

                vector<string>           execs{"clean", "dump", "sim", "simgui", "geopop"};
                ValuesConstraint<string> vc(execs);
                string                   se = "Execute the function selected.\n"
                            "  clean:  cleans the configuration file and writes it to a new file.\n"
                            "  dump:   takes the built-in configuration writes it to a file.\n"
                            "  sim:    runs the simulator and is the default.\n"
                            "  simgui: runs the simulator with a graphical interface.\n"
                            "  geopop: runs the geospatial synthetic population generator\n";
                ValueArg<string> execArg("e", "exec", se, false, "sim", &vc, cmd);

                cmd.parse(argc, static_cast<const char* const*>(argv));

                // -----------------------------------------------------------------------------------------
                // Get configuration and path with overrides (if any).
                // -----------------------------------------------------------------------------------------
                auto  config = configArg.getValue();
                ptree configPt;

                if (regex_search(config, regex("^name="))) {
                        config   = regex_replace(config, regex(string("^name=")), string(""));
                        configPt = RunConfigManager::Create(config);
                } else {
                        config = regex_replace(config, regex(string("^file=")), string(""));
                        const boost::filesystem::path configPath =
                            (installedArg.getValue()) ? FileSys::GetConfigDir() /= config : config;
                        configPt = FileSys::ReadPtreeFile(configPath);
                }

                for (const auto& p_assignment : overrideArg.getValue()) {
                        const auto v = util::Tokenize(p_assignment, "=");
                        configPt.put("run." + v[0], v[1]);
                }

                // -----------------------------------------------------------------------------------------
                // If run simulation in cli ...
                // -----------------------------------------------------------------------------------------
                if (execArg.getValue() == "sim") {
                        if (configPt.get<string>("run.output_prefix", "").empty()) {
                                configPt.put("run.output_prefix", TimeStamp().ToTag().append("/"));
                        }
                        if (stanArg.isSet()) {
                                configPt.put("run.stan_count", stanArg.getValue());
                        }
                        configPt.sort();

                        if (!stanArg.isSet()) {
                                CliController(configPt).Control();
                        } else {
                                StanController(configPt).Control();
                        }
                }
                // -----------------------------------------------------------------------------------------
                // If run simulation in gui ...
                // -----------------------------------------------------------------------------------------
                else if (execArg.getValue() == "simgui") {
                        cout << "Not implemented here yet ..." << endl;
                }
                // -----------------------------------------------------------------------------------------
                // If geopop ...
                // -----------------------------------------------------------------------------------------
                else if (execArg.getValue() == "geopop") {
                        cout << "Not implemented here yet ..." << endl;
                }
                // -----------------------------------------------------------------------------------------
                // If clean/dump ...
                // -----------------------------------------------------------------------------------------
                else if (execArg.getValue() == "clean" || execArg.getValue() == "dump") {
                        RunConfigManager::CleanConfigFile(configPt);
                }
        } catch (exception& e) {
                exitStatus = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exitStatus = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }
        return exitStatus;
}
