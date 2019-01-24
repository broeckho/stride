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

#include "gengeopop/generators/CollegeGenerator.h"
#include "gengeopop/generators/CommunityGenerator.h"
#include "gengeopop/generators/GeoGridPoolBuilder.h"
#include "gengeopop/generators/HouseholdGenerator.h"
#include "gengeopop/generators/K12SchoolGenerator.h"
#include "gengeopop/generators/WorkplaceGenerator.h"
#include "gengeopop/io/CommutesReader.h"
#include "gengeopop/io/ReaderFactory.h"
#include "gengeopop/populators/CollegePopulator.h"
#include "gengeopop/populators/GeoGridPopBuilder.h"
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
                                   stride::util::RnMan& rnManager, string citiesFileName,
                                   string commutingFileName, string householdFileName)
    : m_geoGridConfig(geoGridConfig), m_rnManager(rnManager), m_geoGrid(nullptr),
      m_population(stride::Population::Create()), m_citiesReader(nullptr), m_commutesReader(nullptr),
      m_householdsReader(nullptr), m_logger(move(logger)), m_citiesFileName(move(citiesFileName)),
      m_commutingFileName(move(commutingFileName)), m_householdsFileName(move(householdFileName))
{
}

void GenPopController::ReadDataFiles()
{
        m_geoGrid = make_shared<GeoGrid>(m_population.get());

        ReaderFactory readerFactory;

#pragma omp parallel sections
        {
#pragma omp section
                {
                        m_citiesReader = readerFactory.CreateCitiesReader(m_citiesFileName);
                        m_citiesReader->FillGeoGrid(m_geoGrid);
                }

#pragma omp section
                {
                        if (!m_commutingFileName.empty()) {
                                m_commutesReader = readerFactory.CreateCommutesReader(m_commutingFileName);
                        }
                }

#pragma omp section
                {
                        m_householdsReader = readerFactory.CreateHouseholdReader(m_householdsFileName);
                }
        }

        if (!m_commutingFileName.empty()) {
                m_commutesReader->FillGeoGrid(m_geoGrid);
        }

        m_geoGridConfig.Calculate(m_geoGrid, m_householdsReader);

        m_geoGrid->Finalize();
}

void GenPopController::GenGeo()
{
        GeoGridPoolBuilder geoBuilder(m_geoGridConfig, m_geoGrid);
        geoBuilder.AddGenerator(make_shared<K12SchoolGenerator>(m_rnManager, m_logger));
        geoBuilder.AddGenerator(make_shared<CollegeGenerator>(m_rnManager, m_logger));
        geoBuilder.AddGenerator(make_shared<WorkplaceGenerator>(m_rnManager, m_logger));
        geoBuilder.AddGenerator(make_shared<CommunityGenerator>(m_rnManager, m_logger));
        geoBuilder.AddGenerator(make_shared<HouseholdGenerator>(m_rnManager, m_logger));
        geoBuilder.BuildPools();
}

void GenPopController::GenPop()
{
        GeoGridPopBuilder popBuilder(m_geoGridConfig, m_geoGrid);
        popBuilder.AddPartialPopulator(make_shared<HouseholdPopulator>(m_rnManager, m_logger));
        popBuilder.AddPartialPopulator(make_shared<K12SchoolPopulator>(m_rnManager, m_logger));
        popBuilder.AddPartialPopulator(make_shared<CollegePopulator>(m_rnManager, m_logger));
        popBuilder.AddPartialPopulator(make_shared<PrimaryCommunityPopulator>(m_rnManager, m_logger));
        popBuilder.AddPartialPopulator(make_shared<SecondaryCommunityPopulator>(m_rnManager, m_logger));
        popBuilder.AddPartialPopulator(make_shared<WorkplacePopulator>(m_rnManager, m_logger));
        popBuilder.BuildPop();
}

shared_ptr<GeoGrid> GenPopController::GetGeoGrid() { return m_geoGrid; }

void GenPopController::UsePopulation(shared_ptr<stride::Population> pop) { m_population = move(pop); }

} // namespace gengeopop
