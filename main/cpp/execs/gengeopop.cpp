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
#include "gengeopop/GenGeoPopController.h"
#include "gengeopop/GeoGrid.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/generators/CollegeGenerator.h"
#include "gengeopop/generators/CommunityGenerator.h"
#include "gengeopop/generators/GeoGridGenerator.h"
#include "gengeopop/generators/HouseholdGenerator.h"
#include "gengeopop/generators/K12SchoolGenerator.h"
#include "gengeopop/generators/WorkplaceGenerator.h"
#include "gengeopop/io/GeoGridProtoWriter.h"
#include "gengeopop/io/GeoGridWriterFactory.h"
#include "gengeopop/io/ReaderFactory.h"
#include "gengeopop/populators/CollegePopulator.h"
#include "gengeopop/populators/GeoGridPopulator.h"
#include "gengeopop/populators/HouseholdPopulator.h"
#include "gengeopop/populators/K12SchoolPopulator.h"
#include "gengeopop/populators/PrimaryCommunityPopulator.h"
#include "gengeopop/populators/SecondaryCommunityPopulator.h"
#include "gengeopop/populators/WorkplacePopulator.h"
#include "util/LogUtils.h"

#include <boost/lexical_cast.hpp>
#include <tclap/CmdLine.h>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <spdlog/common.h>
#include <spdlog/fmt/ostr.h>
#include <string>
#include <utility>

using namespace gengeopop;
using namespace TCLAP;
using namespace std;
using namespace stride::util;

int main(int argc, char* argv[])
{
        int exit_status = EXIT_SUCCESS;

        // base structure copied from sim/main.cpp
        try {
                // --------------------------------------------------------------
                // Parse parameters.
                // --------------------------------------------------------------
                CmdLine cmd("gengeopop", ' ', "1.0");

                string sci = "Input file with data on cities in csv format."
                             "Defaults to flanders_cities.csv.";
                ValueArg<string> citiesFile("", "cities", sci, false, "flanders_cities.csv", "CITIES FILE", cmd);

                string sco = "Input file with data on commuting in csv format."
                             "Defaults to flanders_commuting.csv.";
                ValueArg<string> commutingFile("", "commuting", sco, false, "flanders_commuting.csv", "COMMUTING FILE",
                                               cmd);

                string sho = "Input file with reference set of households in csv format."
                             "Defaults to households_flanders.csv.";
                ValueArg<string> houseHoldFile("", "household", sho, false, "households_flanders.csv", "OUTPUT FILE",
                                               cmd);

                string sou = "Output file with synthetic population in protobuf format."
                             "Defaults to gengeopop.proto.";
                ValueArg<string> outputFile("o", "output", sou, false, "gengeopop.proto", "OUTPUT FILE", cmd);

                ValueArg<string> logLevel("l", "loglevel", "Loglevel", false, "info", "LOGLEVEL", cmd);

                ValueArg<double> fraction1826Students("s", "frac1826students",
                                                      "Fraction of 1826 years which are students", false, 0.50,
                                                      "FRACTION STUDENTS (1826)", cmd);

                ValueArg<double> fractionActiveCommutingPeople("t", "fracActiveCommuting",
                                                               "Fraction of active people commuting", false, 0.50,
                                                               "FRACTION OF ACTIVE PEOPLE COMMUTING", cmd);

                ValueArg<double> fractionStudentCommutingPeople("w", "fracStudentCommuting",
                                                                "Fraction of students commuting", false, 0.50,
                                                                "FRACTION OF STUDENTS COMMUTING", cmd);

                ValueArg<double> fractionActivePeople("a", "fracActive", "Fraction of people active", false, 0.75,
                                                      "FRACTION OF PEOPLE ACTIVE", cmd);

                ValueArg<unsigned int> populationSize("p", "populationSize", "Population size", false, 6000000,
                                                      "POPULATION SIZE", cmd);

                ValueArg<string> rng_seed("", "seed", "The seed to be used for the random engine", false, "1,2,3,4",
                                          "SEED", cmd);

                cmd.parse(argc, static_cast<const char* const*>(argv));

                // --------------------------------------------------------------
                // Create logger.
                // --------------------------------------------------------------
                shared_ptr<spdlog::logger> logger = LogUtils::CreateCliLogger("stride_logger", "stride_log.txt");
                logger->set_level(spdlog::level::from_str(logLevel.getValue()));
                logger->flush_on(spdlog::level::err);

                // --------------------------------------------------------------
                // Configure.
                // --------------------------------------------------------------
                GeoGridConfig geoGridConfig{};
                geoGridConfig.input.populationSize                       = populationSize.getValue();
                geoGridConfig.input.fraction_1826_years_WhichAreStudents = fraction1826Students.getValue();
                geoGridConfig.input.fraction_active_commutingPeople      = fractionActiveCommutingPeople.getValue();
                geoGridConfig.input.fraction_student_commutingPeople     = fractionStudentCommutingPeople.getValue();
                geoGridConfig.input.fraction_1865_years_active           = fractionActivePeople.getValue();

                RnMan::Info info(rng_seed.getValue(), "", static_cast<unsigned int>(omp_get_num_threads()));
                RnMan       rnManager(info);

                GenGeoPopController genGeoPopController(logger, geoGridConfig, rnManager, citiesFile.getValue(),
                                                        commutingFile.getValue(), houseHoldFile.getValue());

                // --------------------------------------------------------------
                // Read input files.
                // --------------------------------------------------------------
                genGeoPopController.ReadDataFiles();
                logger->info("GeoGridConfig:\n\n{}", geoGridConfig);
                logger->info("Random engine initialized with seed: {}", info.m_seed_seq_init);
                logger->info("Number of threads: {}", info.m_stream_count);

                // --------------------------------------------------------------
                // Generate Geo
                // --------------------------------------------------------------
                logger->info("Start generation geographic grid.");
                genGeoPopController.GenGeo();
                logger->info("Number of ContactCenters generated: {}", geoGridConfig.generated.contactCenters);
                logger->info("Number of ContactPools generated: {}", geoGridConfig.generated.contactPools);
                logger->info("Done generation geographic grid.");

                // --------------------------------------------------------------
                // Generate Pop
                // --------------------------------------------------------------
                logger->info("Start generating of synthetic population.");
                genGeoPopController.GenPop();
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
