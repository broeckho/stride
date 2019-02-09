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

shared_ptr<Population> GenPopBuilder::Build(shared_ptr<Population> pop)
{
        if (!m_stride_logger) {
                m_stride_logger = LogUtils::CreateNullLogger("PopBuilder_logger");
        }

        // --------------------------------------------------------------
        // Configure.
        // --------------------------------------------------------------
        GeoGridConfig ggConfig(m_config_pt);
        GenPopController genPopController(m_stride_logger, ggConfig, m_rn_manager, pop);

        // --------------------------------------------------------------
        // Read input files (commute info file only if present).
        // --------------------------------------------------------------
        string commutesFile;
        auto geopop_gen = m_config_pt.get_child("run.geopop_gen");
        if (geopop_gen.count("commuting_file")) {
                commutesFile = m_config_pt.get<string>("run.geopop_gen.commuting_file");
        }
        genPopController.ReadDataFiles(m_config_pt.get<string>("run.geopop_gen.cities_file"), commutesFile,
                                          m_config_pt.get<string>("run.geopop_gen.household_file"));

        m_stride_logger->info("GeoGridConfig:\n\n{}", ggConfig);
        m_stride_logger->info("Number of reference households: {}", ggConfig.popInfo.reference_households.size());
        m_stride_logger->info("Number of reference persons: {}", ggConfig.popInfo.persons.size());
        m_stride_logger->info("Number of reference households: {}", ggConfig.popInfo.contact_pools.size());

        // --------------------------------------------------------------
        // Generate Geo
        // --------------------------------------------------------------
        m_stride_logger->info("Starting Gen-Geo");
        genPopController.GenGeo();
        m_stride_logger->info("ContactCenters generated: {}", ggConfig.counters.contact_center_count);
        m_stride_logger->info("Finished Gen-Geo");

        // --------------------------------------------------------------
        // Generate Pop
        // --------------------------------------------------------------
        m_stride_logger->info("Starting Gen-Pop");
        genPopController.GenPop();
        m_stride_logger->info("Finished Gen-Pop");

        pop->m_geoGrid = genPopController.GetGeoGrid();

        return pop;
}

} // namespace stride
