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
#include "geopop/GeoGridConfig.h"
#include "geopop/generators/CollegeGenerator.h"
#include "geopop/generators/HouseholdGenerator.h"
#include "geopop/generators/K12SchoolGenerator.h"
#include "geopop/generators/PrimaryCommunityGenerator.h"
#include "geopop/generators/SecondaryCommunityGenerator.h"
#include "geopop/generators/WorkplaceGenerator.h"
#include "geopop/io/ReaderFactory.h"
#include "geopop/populators/CollegePopulator.h"
#include "geopop/populators/HouseholdPopulator.h"
#include "geopop/populators/K12SchoolPopulator.h"
#include "geopop/populators/PrimaryCommunityPopulator.h"
#include "geopop/populators/SecondaryCommunityPopulator.h"
#include "geopop/populators/WorkplacePopulator.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"

#include "util/RnMan.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <spdlog/logger.h>
#include <contact/ContactType.h>

namespace stride {

using namespace std;
using namespace util;
using namespace ContactType;
using namespace boost::property_tree;
using namespace geopop;

GeoPopBuilder::GeoPopBuilder(const ptree& config, RnMan& rnMan, shared_ptr<spdlog::logger> strideLogger)
    : AbstractPopBuilder(config, rnMan, move(strideLogger))
{
}

shared_ptr<Population> GeoPopBuilder::Build(shared_ptr<Population> pop)
{
        m_stride_logger->trace("Building geopop.");

        // --------------------------------------------------------------
        // Set the GeoGridConfig.
        // --------------------------------------------------------------
        GeoGridConfig ggConfig(m_config);
        ggConfig.SetData(m_config.get<string>("run.geopop_gen.household_file"));

        // --------------------------------------------------------------
        // Get GeoGrid associated with 'pop'.
        // --------------------------------------------------------------
        auto& geoGrid = pop->RefGeoGrid();

        // --------------------------------------------------------------
        // Read cities input files (commute info file only if present).
        // --------------------------------------------------------------
        string commutesFile;
        auto   geopop_gen = m_config.get_child("run.geopop_gen");
        if (geopop_gen.count("commuting_file")) {
                commutesFile = m_config.get<string>("run.geopop_gen.commuting_file");
        }

        m_stride_logger->trace("Starting MakeLocations");
        MakeLocations(geoGrid, ggConfig, m_config.get<string>("run.geopop_gen.cities_file"), commutesFile);
        m_stride_logger->trace("Finished MakeLocations");

        // --------------------------------------------------------------
        // Generate Geo.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting MakeCenters");
        MakeCenters(geoGrid, ggConfig);
        m_stride_logger->trace("Finished MakeCenters");

        // --------------------------------------------------------------
        // Generate Pop.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting MakePersons");
        MakePersons(geoGrid, ggConfig);
        m_stride_logger->trace("Finished MakePersons");

        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
        m_stride_logger->trace("Done building geopop.");

        return pop;
}

void GeoPopBuilder::MakeLocations(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig, const string& citiesFileName,
                                  const string& commutingFileName)
{
        const auto citiesReader = ReaderFactory::CreateCitiesReader(citiesFileName);
        citiesReader->FillGeoGrid(geoGrid);

        if (!commutingFileName.empty()) {
                const auto commutesReader = ReaderFactory::CreateCommutesReader(commutingFileName);
                commutesReader->FillGeoGrid(geoGrid);
        }

        for (const shared_ptr<Location>& loc : geoGrid) {
                loc->SetPopCount(geoGridConfig.input.pop_size);
        }
        geoGrid.Finalize();
}

void GeoPopBuilder::MakeCenters(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        vector<shared_ptr<Generator>> generators{make_shared<K12SchoolGenerator>(m_rn_man, m_stride_logger),
                                                 make_shared<CollegeGenerator>(m_rn_man, m_stride_logger),
                                                 make_shared<WorkplaceGenerator>(m_rn_man, m_stride_logger),
                                                 make_shared<PrimaryCommunityGenerator>(m_rn_man, m_stride_logger),
                                                 make_shared<SecondaryCommunityGenerator>(m_rn_man, m_stride_logger),
                                                 make_shared<HouseholdGenerator>(m_rn_man, m_stride_logger)};

        for (const auto& g : generators) {
                auto ccCounter = 1U;
                g->Apply(geoGrid, geoGridConfig, ccCounter);
        }
}

void GeoPopBuilder::MakePersons(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        vector<shared_ptr<Populator>> populators{make_shared<HouseholdPopulator>(m_rn_man, m_stride_logger),
                                                 make_shared<K12SchoolPopulator>(m_rn_man, m_stride_logger),
                                                 make_shared<CollegePopulator>(m_rn_man, m_stride_logger),
                                                 make_shared<PrimaryCommunityPopulator>(m_rn_man, m_stride_logger),
                                                 make_shared<SecondaryCommunityPopulator>(m_rn_man, m_stride_logger),
                                                 make_shared<WorkplacePopulator>(m_rn_man, m_stride_logger)};

        for (shared_ptr<Populator>& p : populators) {
                p->Apply(geoGrid, geoGridConfig);
        }
}

} // namespace stride
