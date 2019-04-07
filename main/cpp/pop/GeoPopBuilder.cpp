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

#include "geopop/GeoGridBuilder.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/GeoGridConfigBuilder.h"
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
using namespace geopop;

shared_ptr<Population> GeoPopBuilder::Build(shared_ptr<Population> pop)
{
        if (!m_stride_logger) {
                m_stride_logger = LogUtils::CreateNullLogger("PopBuilder_logger");
        }

        // --------------------------------------------------------------
        // Set the GeoGridConfig.
        // --------------------------------------------------------------
        GeoGridConfig        ggConfig(m_config_pt);
        GeoGridConfigBuilder ggConfigBuilder{};
        ggConfigBuilder.SetData(ggConfig, m_config_pt.get<string>("run.geopop_gen.household_file"));

        m_stride_logger->info("Number of reference households: {}", ggConfig.refHH.households.size());
        m_stride_logger->info("Number of reference persons: {}", ggConfig.refHH.persons.size());
        m_stride_logger->info("Number of reference households: {}", ggConfig.refHH.pools.size());

        // --------------------------------------------------------------
        // Read cities input files (commute info file only if present).
        // --------------------------------------------------------------
        string commutesFile;
        auto   geopop_gen = m_config_pt.get_child("run.geopop_gen");
        if (geopop_gen.count("commuting_file")) {
                commutesFile = m_config_pt.get<string>("run.geopop_gen.commuting_file");
        }
        GeoGridBuilder ggBuilder(m_stride_logger, m_rn_manager, pop);

        m_stride_logger->info("Starting GenCities");
        ggBuilder.GenCities(ggConfig, m_config_pt.get<string>("run.geopop_gen.cities_file"), commutesFile);
        m_stride_logger->info("Finished GenCities");

        // --------------------------------------------------------------
        // Generate Geo
        // --------------------------------------------------------------
        m_stride_logger->info("Starting GenGeo");
        ggBuilder.GenGeo(ggConfig);
        m_stride_logger->info("Finished GenGeo");

        // --------------------------------------------------------------
        // Generate Pop
        // --------------------------------------------------------------
        m_stride_logger->info("Starting GenPop");
        ggBuilder.GenPop(ggConfig);
        m_stride_logger->info("Finished GenPop");

        pop->m_geoGrid = ggBuilder.GetGeoGrid();

        return pop;
}

} // namespace stride
