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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "GenPopController.h"

#include "gengeopop/GeoGrid.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/GeoGridConfigBuilder.h"
#include "gengeopop/generators/CollegeGenerator.h"
#include "gengeopop/generators/CommunityGenerator.h"
#include "gengeopop/generators/HouseholdGenerator.h"
#include "gengeopop/generators/K12SchoolGenerator.h"
#include "gengeopop/generators/WorkplaceGenerator.h"
#include "gengeopop/io/ReaderFactory.h"
#include "gengeopop/populators/CollegePopulator.h"
#include "gengeopop/populators/HouseholdPopulator.h"
#include "gengeopop/populators/K12SchoolPopulator.h"
#include "gengeopop/populators/PrimaryCommunityPopulator.h"
#include "gengeopop/populators/SecondaryCommunityPopulator.h"
#include "gengeopop/populators/WorkplacePopulator.h"

#include <spdlog/logger.h>
#include <utility>

namespace gengeopop {

using namespace std;

GenPopController::GenPopController(shared_ptr<spdlog::logger> logger,  stride::util::RnMan& rnManager,
                                   shared_ptr<stride::Population> pop)
    : m_rnManager(rnManager), m_geoGrid(nullptr), m_population(move(pop)),
      m_logger(move(logger))
{
}

void GenPopController::ReadDataFiles(const GeoGridConfig& geoGridConfig,
                                     const string& citiesFileName, const string& commutingFileName)
{
        m_geoGrid = make_shared<GeoGrid>(m_population.get());

        ReaderFactory              readerFactory;
        shared_ptr<CitiesReader>   citiesReader;
        shared_ptr<CommutesReader> commutesReader;

        citiesReader = readerFactory.CreateCitiesReader(citiesFileName);
        citiesReader->FillGeoGrid(m_geoGrid);
        if (!commutingFileName.empty()) {
                commutesReader = readerFactory.CreateCommutesReader(commutingFileName);
                commutesReader->FillGeoGrid(m_geoGrid);
        }

        for (const shared_ptr<Location>& loc : *m_geoGrid) {
                loc->SetPopCount(geoGridConfig.input.pop_size);
        }
        m_geoGrid->Finalize();
}

void GenPopController::GenGeo(const GeoGridConfig& geoGridConfig)
{
        vector<shared_ptr<Generator>> generators{make_shared<K12SchoolGenerator>(m_rnManager, m_logger),
                                                 make_shared<CollegeGenerator>(m_rnManager, m_logger),
                                                 make_shared<WorkplaceGenerator>(m_rnManager, m_logger),
                                                 make_shared<CommunityGenerator>(m_rnManager, m_logger),
                                                 make_shared<HouseholdGenerator>(m_rnManager, m_logger)};

        for (const auto& g : generators) {
                g->Apply(m_geoGrid, geoGridConfig, m_ccCounter);
        }
}

void GenPopController::GenPop(const GeoGridConfig& geoGridConfig)
{
        vector<shared_ptr<Populator>> populators{make_shared<HouseholdPopulator>(m_rnManager, m_logger),
                                                 make_shared<K12SchoolPopulator>(m_rnManager, m_logger),
                                                 make_shared<CollegePopulator>(m_rnManager, m_logger),
                                                 make_shared<PrimaryCommunityPopulator>(m_rnManager, m_logger),
                                                 make_shared<SecondaryCommunityPopulator>(m_rnManager, m_logger),
                                                 make_shared<WorkplacePopulator>(m_rnManager, m_logger)};

        for (shared_ptr<Populator>& p : populators) {
                p->Apply(m_geoGrid, geoGridConfig);
        }
}

shared_ptr<GeoGrid> GenPopController::GetGeoGrid() { return m_geoGrid; }

} // namespace gengeopop
