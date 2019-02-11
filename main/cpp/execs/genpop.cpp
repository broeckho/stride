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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */


#include "gengeopop/GeoGrid.h"
#include "gengeopop/io/GeoGridWriter.h"
#include "gengeopop/io/GeoGridWriterFactory.h"
#include "pop/GenPopBuilder.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/RunConfigManager.h"

#include <boost/property_tree/ptree.hpp>
#include <tclap/CmdLine.h>
#include <spdlog/common.h>
#include <spdlog/fmt/ostr.h>

#include <fstream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

using namespace gengeopop;
using namespace TCLAP;
using namespace std;
using namespace stride;
using namespace stride::util;
using namespace boost::property_tree;

int main(int argc, char* argv[])
{
        int exit_status = EXIT_SUCCESS;

        try {
                // --------------------------------------------------------------
                // Parse parameters.
                // --------------------------------------------------------------
                CmdLine cmd("genpop", ' ', "1.0");

                string si = "Look for configuration file specified by the -c file=<file> "
                            " or -c <file> in the stride install directories";
                SwitchArg installedArg("i", "installed", si, cmd, true);

                string           so = "Override configuration file parameters with values provided here.";
                MultiArg<string> overrideArg("o", "override", so, false, "<NAME>=<VALUE>", cmd);

                string sc = "Specifies the run configuration parameters. The format may be "
                            "either -c file=<file> or -c name=<name>. The first is most "
                            "used and may be shortened to -c <file>. The second refers to "
                            "built-in configurations specified by their name."
                            "\nDefaults to -c file=run_default.xml";
                ValueArg<string> configArg("c", "config", sc, false, "run_generate_default.xml", "CONFIGURATION", cmd);

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
                        const filesys::path configPath =
                            (installedArg.getValue()) ? FileSys::GetConfigDir() /= config : filesys::path(config);
                        configPt = FileSys::ReadPtreeFile(configPath);
                }

                for (const auto& p_assignment : overrideArg.getValue()) {
                        const auto v = stride::util::Tokenize(p_assignment, "=");
                        configPt.put("run." + v[0], v[1]);
                }

                // --------------------------------------------------------------
                // Create logger.
                // --------------------------------------------------------------
                shared_ptr<spdlog::logger> logger   = LogUtils::CreateCliLogger("stride_logger", "stride_log.txt");
                const auto                 logLevel = configPt.get<string>("run.stride_log_level");
                logger->set_level(spdlog::level::from_str(logLevel));
                logger->flush_on(spdlog::level::err);

                // --------------------------------------------------------------
                // Create random number manager.
                // --------------------------------------------------------------
                RnMan::Info info{configPt.get<string>("pop.rng_seed", "1,2,3,4"), "",
                                 configPt.get<unsigned int>("run.num_threads")};
                RnMan       rnManager(info);
                logger->info("Random engine initialized with seed: {}", info.m_seed_seq_init);
                logger->info("Number of threads: {}", info.m_stream_count);

                // --------------------------------------------------------------
                // Set up the GenPopBuilder and build population with GeoGrid.
                // --------------------------------------------------------------
                logger->info("GenPopBuilder invoked.");
                GenPopBuilder genPopBuilder(configPt, rnManager, logger);
                const auto pop = Population::Create();

                genPopBuilder.Build(pop);
                logger->info("GenPopBuilder done.");

                // --------------------------------------------------------------
                // Write to file.
                // --------------------------------------------------------------
                logger->info("Writing to population file.");
                GeoGridWriterFactory      geoGridWriterFactory;
                const auto                popFileName = configPt.get<string>("run.population_file", "gengeopop.proto");
                shared_ptr<GeoGridWriter> geoGridWriter = geoGridWriterFactory.CreateWriter(popFileName);
                ofstream                  outputFileStream(popFileName);

                geoGridWriter->Write(pop->GetGeoGrid(), outputFileStream);
                outputFileStream.close();
                logger->info("Done writing to population to file {}.", popFileName);

        } catch (exception& e) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }
        return exit_status;
}
