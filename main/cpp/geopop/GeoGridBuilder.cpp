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
 *  Copyright 2019, Jan Broeckhove and Bistromatics group.
 */

#include "GeoGridBuilder.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/GeoGridConfigBuilder.h"
#include "geopop/generators/CollegeGenerator.h"
#include "geopop/generators/CommunityGenerator.h"
#include "geopop/generators/HouseholdGenerator.h"
#include "geopop/generators/K12SchoolGenerator.h"
#include "geopop/generators/WorkplaceGenerator.h"
#include "geopop/io/ReaderFactory.h"
#include "geopop/populators/CollegePopulator.h"
#include "geopop/populators/HouseholdPopulator.h"
#include "geopop/populators/K12SchoolPopulator.h"
#include "geopop/populators/PrimaryCommunityPopulator.h"
#include "geopop/populators/SecondaryCommunityPopulator.h"
#include "geopop/populators/WorkplacePopulator.h"

#include <spdlog/logger.h>
#include <utility>

namespace geopop {

using namespace std;

GeoGridBuilder::GeoGridBuilder(shared_ptr<spdlog::logger> logger, stride::util::RnMan& rnManager)
    : m_rnManager(rnManager), m_logger(move(logger))
{
}

void GeoGridBuilder::GenCities(const std::shared_ptr<GeoGrid>& geoGrid, const GeoGridConfig& geoGridConfig,
                               const string& citiesFileName, const string& commutingFileName)
{
        ReaderFactory              readerFactory;
        shared_ptr<CitiesReader>   citiesReader;
        shared_ptr<CommutesReader> commutesReader;

        citiesReader = readerFactory.CreateCitiesReader(citiesFileName);
        citiesReader->FillGeoGrid(geoGrid);
        if (!commutingFileName.empty()) {
                commutesReader = readerFactory.CreateCommutesReader(commutingFileName);
                commutesReader->FillGeoGrid(geoGrid);
        }

        for (const shared_ptr<Location>& loc : *geoGrid) {
                loc->SetPopCount(geoGridConfig.input.pop_size);
        }
        geoGrid->Finalize();
}

void GeoGridBuilder::GenGeo(const std::shared_ptr<GeoGrid>& geoGrid, const GeoGridConfig& geoGridConfig)
{
        vector<shared_ptr<Generator>> generators{make_shared<K12SchoolGenerator>(m_rnManager, m_logger),
                                                 make_shared<CollegeGenerator>(m_rnManager, m_logger),
                                                 make_shared<WorkplaceGenerator>(m_rnManager, m_logger),
                                                 make_shared<CommunityGenerator>(m_rnManager, m_logger),
                                                 make_shared<HouseholdGenerator>(m_rnManager, m_logger)};

        for (const auto& g : generators) {
                g->Apply(geoGrid, geoGridConfig, m_ccCounter);
        }
}

void GeoGridBuilder::GenPop(const std::shared_ptr<GeoGrid>& geoGrid, const GeoGridConfig& geoGridConfig)
{
        vector<shared_ptr<Populator>> populators{make_shared<HouseholdPopulator>(m_rnManager, m_logger),
                                                 make_shared<K12SchoolPopulator>(m_rnManager, m_logger),
                                                 make_shared<CollegePopulator>(m_rnManager, m_logger),
                                                 make_shared<PrimaryCommunityPopulator>(m_rnManager, m_logger),
                                                 make_shared<SecondaryCommunityPopulator>(m_rnManager, m_logger),
                                                 make_shared<WorkplacePopulator>(m_rnManager, m_logger)};

        for (shared_ptr<Populator>& p : populators) {
                p->Apply(geoGrid, geoGridConfig);
        }
}

} // namespace geopop
