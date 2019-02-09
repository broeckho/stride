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

#include "gengeopop/Community.h"
#include "gengeopop/GenPopController.h"
#include "gengeopop/GeoGrid.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/GeoGridConfigBuilder.h"
#include "gengeopop/io/GeoGridProtoWriter.h"
#include "gengeopop/io/GeoGridWriterFactory.h"
#include "gengeopop/io/ReaderFactory.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/RunConfigManager.h"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tclap/CmdLine.h>
#include <omp.h>
#include <spdlog/common.h>
#include <spdlog/fmt/ostr.h>

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

using namespace gengeopop;
using namespace TCLAP;
using namespace std;
using namespace stride::util;
using namespace boost::property_tree;

int main(int argc, char* argv[])
{
        int exit_status = EXIT_SUCCESS;

        // base structure copied from sim/main.cpp
        try {
                // --------------------------------------------------------------
                // Parse parameters.
                // --------------------------------------------------------------
                CmdLine cmd("genpop", ' ', "1.0");

                string sou = "Output file with synthetic population in protobuf format.                    "
                             "Defaults to --output gengeopop.proto.";
                ValueArg<string> outputFile("", "output", sou, false, "gengeopop.proto", "OUTPUT FILE", cmd);

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
                // Set the GeoGridConfig.
                // --------------------------------------------------------------
                GeoGridConfig ggConfig(configPt);
                GeoGridConfigBuilder ggConfigBuilder{};
                ggConfigBuilder.SetData(ggConfig, configPt.get<string>("run.geopop_gen.household_file"));

                logger->info("Number of reference households: {}", ggConfig.popInfo.reference_households.size());
                logger->info("Number of reference persons: {}", ggConfig.popInfo.persons.size());
                logger->info("Number of reference households: {}", ggConfig.popInfo.contact_pools.size());

                // --------------------------------------------------------------
                // Read input files (commutesFile may be absent).
                // --------------------------------------------------------------
                string commutesFile;
                // Check if given
                auto geopop_gen = configPt.get_child("run.geopop_gen");
                if (geopop_gen.count("commuting_file")) {
                        commutesFile = configPt.get<std::string>("run.geopop_gen.commuting_file");
                }
                GenPopController genGeoPopController(logger, rnManager);
                genGeoPopController.ReadDataFiles(ggConfig, configPt.get<string>("run.geopop_gen.cities_file"),
                                                  commutesFile);

                // --------------------------------------------------------------
                // Generate Geo
                // --------------------------------------------------------------
                logger->info("Start generation geographic grid.");
                genGeoPopController.GenGeo(ggConfig);
                logger->info("Done generation geographic grid.");

                // --------------------------------------------------------------
                // Generate Pop
                // --------------------------------------------------------------
                logger->info("Start generating of synthetic population.");
                genGeoPopController.GenPop(ggConfig);
                logger->info("Done generating synthetic population.");

                // --------------------------------------------------------------
                // Write to file.
                // --------------------------------------------------------------
                logger->info("Writing to population file.");
                GeoGridWriterFactory      geoGridWriterFactory;
                shared_ptr<GeoGridWriter> geoGridWriter = geoGridWriterFactory.CreateWriter(outputFile.getValue());
                ofstream                  outputFileStream(outputFile.getValue());
                geoGridWriter->Write(genGeoPopController.GetGeoGrid(), outputFileStream);
                outputFileStream.close();
                logger->info("Done writing to population file.");

        } catch (exception& e) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exit_status = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }
        return exit_status;
}
