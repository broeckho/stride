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
 *  Copyright 2017, Draulans S, Van Leeuwen L.
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/*
 * @file
 * Main program for the society generator.
 */

#include "AliasMethod.h"
#include "SocietyCSVWriter.h"
#include "SocietyGenerator.h"
#include "util/ConfigInfo.h"
#include "util/InstallDirs.h"
#include "util/TimeStamp.h"

#include <tclap/CmdLine.h>
#include <omp.h>

using namespace std;
using namespace stride::generator;
using namespace stride::util;
using namespace TCLAP;
using namespace boost::filesystem;

using boost::filesystem::path;

int main(int argc, char* argv[])
{
        int exit_status = EXIT_SUCCESS;
        try {
                // -----------------------------------------------------------------------------------------
                // Starting up ....
                // -----------------------------------------------------------------------------------------
                cout << "\n*************************************************************" << endl;
                cout << "Starting up at:      " << TimeStamp().ToString() << endl << endl;

                // -----------------------------------------------------------------------------------------
                // Parse command line.
                // -----------------------------------------------------------------------------------------
                CmdLine          cmd("stride", ' ', "1.0", false);
                ValueArg<string> config_file_Arg("c", "config", "Config File", false, "run_generator_default.xml",
                                                 "CONFIGURATION FILE", cmd);
                cmd.parse(argc, static_cast<const char* const*>(argv));
                string config_file_name = config_file_Arg.getValue();

                // -----------------------------------------------------------------------------------------
                // Check exec environment and configuration file
                // -----------------------------------------------------------------------------------------
                InstallDirs::Print(cout);
                InstallDirs::Check();
                const path file_path = InstallDirs::GetConfigDir() / path(config_file_name);
                if (!is_regular_file(file_path)) {
                        throw runtime_error(string(__func__) + ">Config file " + file_path.string() +
                                            " not present. Aborting.");
                }

                // -----------------------------------------------------------------------------------------
                // Parallellization
                // -----------------------------------------------------------------------------------------
                unsigned int num_threads = 1;
                if (ConfigInfo::HaveOpenMP()) {
#pragma omp parallel
                        {
                                num_threads = static_cast<unsigned int>(omp_get_num_threads());
                        }
                        cout << "\nUsing OpenMP threads:  " << num_threads << endl;
                } else {
                        cout << "\nNot using parallellization" << endl;
                }

                // -----------------------------------------------------------------------------------------
                // Run society generator
                // -----------------------------------------------------------------------------------------
                cout << "\nGenerating population\n";
                SocietyGenerator popgen(file_path, num_threads);
                Society          gen_society = popgen.Generate();

                // -----------------------------------------------------------------------------------------
                // Write the population to CSV files
                // -----------------------------------------------------------------------------------------
                const path persons_path     = InstallDirs::GetDataDir() / path("generated_population.csv");
                const path cities_path      = InstallDirs::GetDataDir() / path("generated_cities.csv");
                const path communities_path = InstallDirs::GetDataDir() / path("generated_communities.csv");
                const path households_path  = InstallDirs::GetDataDir() / path("generated_households.csv");
                const auto p_use_xy{popgen.GetConfig().get<bool>("geoprofile.use_xy", false)};

                SocietyCSVWriter popwriter(gen_society, p_use_xy);
                popwriter.WritePersons(persons_path);
                popwriter.WriteCities(cities_path);
                popwriter.WriteCommunities(communities_path);
                popwriter.WriteHouseholds(households_path);
                cout << "Persons      written to: \t" << persons_path << endl;
                cout << "Cities       written to: \t" << cities_path << endl;
                cout << "Communities  written to: \t" << communities_path << endl;
                cout << "Households   written to: \t" << households_path << endl;

                // -----------------------------------------------------------------------------------------
                // Finished
                // -----------------------------------------------------------------------------------------
                cout << "\nFinished at:      " << TimeStamp().ToString() << endl;
                cout << "*************************************************************" << endl;

        } catch (std::exception& e) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPTION THROWN: " << e.what() << endl;
        } catch (...) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPTION THROWN: "
                     << "Unknown exception." << endl;
        }
        return exit_status;
}
