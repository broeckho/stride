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

/**
 * @file
 * Initialize populations: implementation.
 */

#include "GenPopBuilder.h"

#include "gengeopop/GenPopController.h"
#include "gengeopop/GeoGridConfig.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <spdlog/common.h>
#include <spdlog/fmt/ostr.h>

namespace stride {

using namespace std;
using namespace util;
using namespace boost::property_tree;
using namespace gengeopop;

shared_ptr<Population> GenPopBuilder::Build(std::shared_ptr<Population> pop)
{
        auto stride_logger = spdlog::get("stride_logger");
        if (!stride_logger)
                stride_logger = LogUtils::CreateNullLogger("stride_logger");

        // --------------------------------------------------------------
        // Configure.
        // --------------------------------------------------------------
        GeoGridConfig geoGridConfig(m_config_pt);
        GenPopController genGeoPopController(stride_logger, geoGridConfig, m_rn_manager);
        genGeoPopController.UsePopulation(pop);

        // --------------------------------------------------------------
        // Read input files.
        // --------------------------------------------------------------
        std::string commutesFile;
        // Check if given
        auto geopop_gen = m_config_pt.get_child("run.geopop_gen");
        if (geopop_gen.count("commuting_file")) {
                commutesFile = m_config_pt.get<std::string>("run.geopop_gen.commuting_file");
        }
        genGeoPopController.ReadDataFiles(m_config_pt.get<std::string>("run.geopop_gen.cities_file"), commutesFile,
                                          m_config_pt.get<std::string>("run.geopop_gen.household_file"));

        stride_logger->info("GeoGridConfig:\n\n{}", geoGridConfig);

        // --------------------------------------------------------------
        // Generate Geo
        // --------------------------------------------------------------
        stride_logger->info("Starting Gen-Geo");
        genGeoPopController.GenGeo();
        stride_logger->info("ContactCenters generated: {}", geoGridConfig.generated.contactCenters);
        stride_logger->info("Finished Gen-Geo");

        // --------------------------------------------------------------
        // Generate Pop
        // --------------------------------------------------------------
        stride_logger->info("Starting Gen-Pop");
        genGeoPopController.GenPop();
        stride_logger->info("Finished Gen-Pop");

        pop->m_geoGrid = genGeoPopController.GetGeoGrid();

        return pop;
}

} // namespace stride
