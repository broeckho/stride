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

/**
 * @file
 * Initialize populations: implementation.
 */

#include "GeoPopBuilder.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridBuilder.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/GeoGridConfigBuilder.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"

#include "util/RnMan.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <spdlog/logger.h>

namespace stride {

using namespace std;
using namespace util;
using namespace boost::property_tree;
using namespace geopop;

shared_ptr<Population> GeoPopBuilder::Build(shared_ptr<Population> pop)
{
        m_stride_logger->trace("Building geopop.");
        
        // --------------------------------------------------------------
        // Set the GeoGridConfig.
        // --------------------------------------------------------------
        GeoGridConfig        ggConfig(m_config);
        GeoGridConfigBuilder ggConfigBuilder{};
        ggConfigBuilder.SetData(ggConfig, m_config.get<string>("run.geopop_gen.household_file"));

        // --------------------------------------------------------------
        // Create empty GeoGrid associated with 'pop'.
        // --------------------------------------------------------------
        const auto geoGrid = make_shared<GeoGrid>(pop.get());
        
        // --------------------------------------------------------------
        // Read cities input files (commute info file only if present).
        // --------------------------------------------------------------
        string commutesFile;
        auto   geopop_gen = m_config.get_child("run.geopop_gen");
        if (geopop_gen.count("commuting_file")) {
                commutesFile = m_config.get<string>("run.geopop_gen.commuting_file");
        }
        GeoGridBuilder ggBuilder(m_stride_logger, m_rn_man);

        m_stride_logger->trace("Starting GenCities");
        ggBuilder.GenCities(geoGrid, ggConfig, m_config.get<string>("run.geopop_gen.cities_file"), commutesFile);
        m_stride_logger->trace("Finished GenCities");

        // --------------------------------------------------------------
        // Generate Geo.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting GenGeo");
        ggBuilder.GenGeo(geoGrid, ggConfig);
        m_stride_logger->trace("Finished GenGeo");

        // --------------------------------------------------------------
        // Generate Pop.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting GenPop");
        ggBuilder.GenPop(geoGrid, ggConfig);
        m_stride_logger->trace("Finished GenPop");


        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
        pop->m_geo_grid = geoGrid;
        m_stride_logger->trace("Done building geopop.");
        
        return pop;
}

} // namespace stride
