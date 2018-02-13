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

#include "PopulationGenerator.cpp"

#include <tclap/CmdLine.h>
#include <memory>
#include <random>

using namespace std;
using namespace stride;
using namespace popgen;
using namespace TCLAP;

int main(int argc, char** argv)
{
        try {
                // TCLAP commandline interface
                CmdLine cmd("Commandline interface of the PopulationGenerator", ' ', "Beta");

                ValueArg<string> sourceArg("i", "input", "Input xml file for the configuration of the generator", true,
                                           "data/happy_day.xml", "string", cmd);
                ValueArg<string> outputPrefixArg("o", "output", "Output prefix", true, "pop", "string", cmd);
                string           options = "The random generator (one of the following): ";
                options += "default_random_engine - mt19937 - mt19937_64 - minstd_rand0 - minstd_rand - ranlux24_base "
                           "- ranlux48_base - ranlux24 - ranlux48 - knuth_b";
                ValueArg<string> rngArg("r", "randomgenerator", options, false, "mt19937", "string", cmd);

                // The seed argument
                ValueArg<int> seedArg("s", "seed", "The seed of the random generator", false, 1, "int");
                cmd.add(seedArg);

                // Parse the argv array
                cmd.parse(argc, argv);

                // Get the value parsed by each argument
                string sourceXml = sourceArg.getValue();
                string prefix    = outputPrefixArg.getValue();
                string rng       = rngArg.getValue();
                int    seed      = seedArg.getValue();

                cerr << "Starting...\n";
                if (rng == "default_random_engine") {
                        PopulationGenerator<default_random_engine> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "mt19937") {
                        PopulationGenerator<mt19937> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "mt19937_64") {
                        PopulationGenerator<mt19937_64> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "minstd_rand0") {
                        PopulationGenerator<minstd_rand0> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "minstd_rand") {
                        PopulationGenerator<minstd_rand> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "ranlux24_base") {
                        PopulationGenerator<ranlux24_base> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "ranlux48_base") {
                        PopulationGenerator<ranlux48_base> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "ranlux24") {
                        PopulationGenerator<ranlux24> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "ranlux48") {
                        PopulationGenerator<ranlux48> generator{sourceXml, seed};
                        generator.generate(prefix);
                } else if (rng == "knuth_b") {
                        PopulationGenerator<knuth_b> generator{sourceXml, seed};
                        generator.generate(prefix);
                }
                cerr << "Done!\n";
        } catch (ArgException& exc) {
                cerr << "Error: " << exc.error() << " for argument " << exc.argId() << endl;
        }

        return 0;
}
