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

#include "GenGeoPopController.h"
#include <gengeopop/generators/CollegeGenerator.h>
#include <gengeopop/generators/CommunityGenerator.h>
#include <gengeopop/generators/GeoGridGenerator.h>
#include <gengeopop/generators/HouseholdGenerator.h>
#include <gengeopop/generators/K12SchoolGenerator.h>
#include <gengeopop/generators/WorkplaceGenerator.h>
#include <gengeopop/io/ReaderFactory.h>
#include <gengeopop/populators/CollegePopulator.h>
#include <gengeopop/populators/GeoGridPopulator.h>
#include <gengeopop/populators/HouseholdPopulator.h>
#include <gengeopop/populators/K12SchoolPopulator.h>
#include <gengeopop/populators/PrimaryCommunityPopulator.h>
#include <gengeopop/populators/SecondaryCommunityPopulator.h>
#include <gengeopop/populators/WorkplacePopulator.h>
#include <spdlog/logger.h>
#include <utility>
namespace gengeopop {

GenGeoPopController::GenGeoPopController(std::shared_ptr<spdlog::logger> logger, GeoGridConfig& geoGridConfig,
                                         stride::util::RnMan& rnManager, std::string citiesFileName,
                                         std::string commutingFileName, std::string householdFileName)
    : m_geoGridConfig(geoGridConfig), m_rnManager(rnManager), m_geoGrid(nullptr),
      m_population(stride::Population::Create()), m_citiesReader(nullptr), m_commutesReader(nullptr),
      m_householdsReader(nullptr), m_logger(std::move(logger)), m_citiesFileName(std::move(citiesFileName)),
      m_commutingFileName(std::move(commutingFileName)), m_householdsFileName(std::move(householdFileName))
{
}

void GenGeoPopController::ReadDataFiles()
{
        m_geoGrid = std::make_shared<GeoGrid>(m_population.get());

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
                        if (m_commutingFileName != "") {
                                m_commutesReader = readerFactory.CreateCommutesReader(m_commutingFileName);
                        }
                }

#pragma omp section
                {
                        m_householdsReader = readerFactory.CreateHouseholdReader(m_householdsFileName);
                }
        }

        if (m_commutingFileName != "") {
                m_commutesReader->FillGeoGrid(m_geoGrid);
        }

        m_geoGridConfig.Calculate(m_geoGrid, m_householdsReader);

        m_geoGrid->Finalize();
}

void GenGeoPopController::GenGeo()
{
        GeoGridGenerator geoGridGenerator(m_geoGridConfig, m_geoGrid);
        geoGridGenerator.AddPartialGenerator(std::make_shared<K12SchoolGenerator>(m_rnManager, m_logger));
        geoGridGenerator.AddPartialGenerator(std::make_shared<CollegeGenerator>(m_rnManager, m_logger));
        geoGridGenerator.AddPartialGenerator(std::make_shared<WorkplaceGenerator>(m_rnManager, m_logger));
        geoGridGenerator.AddPartialGenerator(std::make_shared<CommunityGenerator>(m_rnManager, m_logger));
        geoGridGenerator.AddPartialGenerator(std::make_shared<HouseholdGenerator>(m_rnManager, m_logger));
        geoGridGenerator.GenerateGeoGrid();
}

void GenGeoPopController::GenPop()
{
        GeoGridPopulator geoGridPopulator(m_geoGridConfig, m_geoGrid);
        geoGridPopulator.AddPartialPopulator(std::make_shared<HouseholdPopulator>(m_rnManager, m_logger));
        geoGridPopulator.AddPartialPopulator(std::make_shared<K12SchoolPopulator>(m_rnManager, m_logger));
        geoGridPopulator.AddPartialPopulator(std::make_shared<CollegePopulator>(m_rnManager, m_logger));
        geoGridPopulator.AddPartialPopulator(std::make_shared<PrimaryCommunityPopulator>(m_rnManager, m_logger));
        geoGridPopulator.AddPartialPopulator(std::make_shared<SecondaryCommunityPopulator>(m_rnManager, m_logger));
        geoGridPopulator.AddPartialPopulator(std::make_shared<WorkplacePopulator>(m_rnManager, m_logger));
        geoGridPopulator.PopulateGeoGrid();
}

std::shared_ptr<GeoGrid> GenGeoPopController::GetGeoGrid() { return m_geoGrid; }

void GenGeoPopController::UsePopulation(std::shared_ptr<stride::Population> pop) { m_population = std::move(pop); }

} // namespace gengeopop
