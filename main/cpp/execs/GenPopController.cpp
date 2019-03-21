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
 * Header for the command line controller.
 */

#include "GenPopController.h"

#include "geopop/GeoGrid.h"
#include "geopop/io/GeoGridWriter.h"
#include "geopop/io/GeoGridWriterFactory.h"

#include "pop/GeoPopBuilder.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#ifdef BOOST_FOUND
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace filesys = boost::filesystem;
#else
#include <filesystem>
namespace filesys = std::filesystem;
#endif

#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <memory>
#include <spdlog/spdlog.h>

using namespace std;
using namespace stride::util;
using namespace geopop;
using namespace boost::property_tree;

namespace stride {

GenPopController::GenPopController(const ptree& config) : ControlHelper("GenPopController", config) {}

void GenPopController::Control()
{
        // -----------------------------------------------------------------------------------------
        // Prelims.
        // -----------------------------------------------------------------------------------------
        CheckEnv();
        CheckOutputPrefix();
        InstallLogger();
        LogStartup();

        // -----------------------------------------------------------------------------------------
        // MakePersons scenario: step 1, build a random number manager.
        // -----------------------------------------------------------------------------------------
        const RnInfo info{m_config.get<string>("pop.rng_seed", "1,2,3,4"), "",
                          m_config.get<unsigned int>("run.num_threads")};
        RnMan        rnMan{info};

        // -----------------------------------------------------------------------------------------
        // MakePersons scenario: step 2, set up the GenPopBuilder and build population with GeoGrid.
        // -----------------------------------------------------------------------------------------
        m_stride_logger->trace("GenPopBuilder invoked.");

        GeoPopBuilder geoPopBuilder(m_config, rnMan, m_stride_logger);
        const auto    pop = Population::Create();
        geoPopBuilder.Build(pop);

        m_stride_logger->trace("GenPopBuilder done.");

        // ----------------------------------------------------------------------------------------
        // MakePersons scenario: step 3, write to population to file.
        // ----------------------------------------------------------------------------------------
        m_stride_logger->trace("Start writing population to file.");

        const auto prefix      = m_config.get<string>("run.output_prefix");
        const auto popFileName = m_config.get<string>("run.population_file", "gengeopop.proto");
        const auto popFilePath = FileSys::BuildPath(prefix, popFileName);
        m_stride_logger->info("Population written to file {}.", popFilePath.string());
        shared_ptr<GeoGridWriter> geoGridWriter = GeoGridWriterFactory::CreateGeoGridWriter(popFileName);
        ofstream                  outputFileStream(popFilePath.string());
        geoGridWriter->Write(pop->RefGeoGrid(), outputFileStream);
        outputFileStream.close();

        m_stride_logger->trace("Done writing population to file.");

        // -----------------------------------------------------------------------------------------
        // Done, shutdown.
        // -----------------------------------------------------------------------------------------
        LogShutdown();
        spdlog::drop_all();
}

} // namespace stride
