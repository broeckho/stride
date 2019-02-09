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

GenPopController::GenPopController(shared_ptr<spdlog::logger> logger, GeoGridConfig& geoGridConfig,
                                   stride::util::RnMan& rnManager, shared_ptr<stride::Population> pop)
    : m_geoGridConfig(geoGridConfig), m_rnManager(rnManager), m_geoGrid(nullptr),
      m_population(move(pop)), m_logger(move(logger))
{
}

void GenPopController::ReadDataFiles(const string& citiesFileName, const string& commutingFileName,
                                     const string& householdsFileName)
{
        m_geoGrid = make_shared<GeoGrid>(m_population.get());

        shared_ptr<CitiesReader>    citiesReader;
        shared_ptr<CommutesReader>  commutesReader;

        ReaderFactory readerFactory;
        GeoGridConfigBuilder  ggConfigBuilder{};
        ggConfigBuilder.SetData(m_geoGridConfig, householdsFileName);

#pragma omp parallel sections
        {
#pragma omp section
                {
                        citiesReader = readerFactory.CreateCitiesReader(citiesFileName);
                        citiesReader->FillGeoGrid(m_geoGrid);
                }

#pragma omp section
                {
                        if (!commutingFileName.empty()) {
                                commutesReader = readerFactory.CreateCommutesReader(commutingFileName);
                        }
                }
        }

        if (!commutingFileName.empty()) {
                commutesReader->FillGeoGrid(m_geoGrid);
        }

        for (const shared_ptr<Location>& loc : *m_geoGrid) {
                loc->SetPopCount(m_geoGridConfig.input.pop_size);
        }
        m_geoGrid->Finalize();
}

void GenPopController::GenGeo()
{
        vector<shared_ptr<Generator>> generators {
                make_shared<K12SchoolGenerator>(m_rnManager, m_logger),
                make_shared<CollegeGenerator>(m_rnManager, m_logger),
                make_shared<WorkplaceGenerator>(m_rnManager, m_logger),
                make_shared<CommunityGenerator>(m_rnManager, m_logger),
                make_shared<HouseholdGenerator>(m_rnManager, m_logger)
        };

        for (const auto& g : generators) {
                g->Apply(m_geoGrid, m_geoGridConfig);
        }
}

void GenPopController::GenPop()
{
        vector<shared_ptr<Populator>> populators {
                make_shared<HouseholdPopulator>(m_rnManager, m_logger),
                make_shared<K12SchoolPopulator>(m_rnManager, m_logger),
                make_shared<CollegePopulator>(m_rnManager, m_logger),
                make_shared<PrimaryCommunityPopulator>(m_rnManager, m_logger),
                make_shared<SecondaryCommunityPopulator>(m_rnManager, m_logger),
                make_shared<WorkplacePopulator>(m_rnManager, m_logger)
        };

        for (shared_ptr<Populator>& p : populators) {
                p->Apply(m_geoGrid, m_geoGridConfig);
        }

}

shared_ptr<GeoGrid> GenPopController::GetGeoGrid() { return m_geoGrid; }

} // namespace gengeopop
