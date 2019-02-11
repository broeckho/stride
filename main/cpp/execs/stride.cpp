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

#include <boost/property_tree/ptree.hpp>
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

                string sa = "Stochastic Analysis (stan) will run <COUNT> simulations, each with a "
                            "different seed for the random engine. Only applies in case of -e sim. ";
                ValueArg<unsigned int> stanArg("", "stan", sa, false, 0, "COUNT", cmd);

                string           so = "Override configuration file parameters with values provided here.";
                MultiArg<string> overrideArg("o", "override", so, false, "<NAME>=<VALUE>", cmd);

                string sc = "Specifies the run configuration parameters. The format may be "
                            "either -c file=<file> or -c name=<name>. The first is most "
                            "used and may be shortened to -c <file>. The second refers to "
                            "built-in configurations specified by their name."
                            "\nDefaults to -c file=run_default.xml";
                ValueArg<string> configArg("c", "config", sc, false, "run_default.xml", "CONFIGURATION", cmd);

                vector<string>           execs{"clean", "dump", "sim", "geopop"};
                ValuesConstraint<string> vc(execs);
                string                   se = "Execute the corresponding function:"
                            "  \n\t clean:  cleans configuration and writes it to a new file."
                            "  \n\t dump:   takes built-in configuration writes it to a file."
                            "  \n\t sim:    runs the simulator and is the default."
                            "  \n\t genpop: saves synthetic population to file; no simulation"
                            "Defaults to --exec sim.";
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
                        const filesys::path configPath = FileSys::GetConfigDir() /= config;
                        configPt = FileSys::ReadPtreeFile(configPath);
                }

                for (const auto& p_assignment : overrideArg.getValue()) {
                        const auto v = util::Tokenize(p_assignment, "=");
                        configPt.put("run." + v[0], v[1]);
                }

                // -----------------------------------------------------------------------------------------
                // If sim ...  fix config and then run simulation in cli ...
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
                // If geopop ...
                // -----------------------------------------------------------------------------------------
                else if (execArg.getValue() == "genpop") {
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
