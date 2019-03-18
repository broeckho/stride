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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "geopop/populators/WorkplacePopulator.h"

#include "contact/AgeBrackets.h"
#include "createGeogrid.h"
#include "geopop/CollegeCenter.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/K12SchoolCenter.h"
#include "geopop/Location.h"
#include "geopop/WorkplaceCenter.h"
#include "geopop/populators/CollegePopulator.h"
#include "pop/Population.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

TEST(WorkplacePopulatorTest, NoPopulation)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        geoGrid.AddLocation(make_shared<Location>(0, 0, Coordinate(0.0, 0.0), "", 0));
        geoGrid.Finalize();

        auto               rnMan = RnMan{RnInfo{}};
        WorkplacePopulator workplacePopulator(rnMan);
        GeoGridConfig      config{};

        EXPECT_NO_THROW(workplacePopulator.Apply(geoGrid, config));
}

TEST(WorkplacePopulatorTest, NoActive)
{
        auto pop = Population::Create();
        SetupGeoGrid(3, 100, 3, 33, 3, pop.get());
        auto& geoGrid = pop->RefGeoGrid();
        auto  rnMan   = RnMan(RnInfo{});

        WorkplacePopulator workplacePopulator(rnMan);
        GeoGridConfig      config{};
        config.input.particpation_workplace = 0;
        config.input.participation_college  = 1;

        auto location = *geoGrid.begin();
        // Brasschaat and Schoten are close to each other
        // There is no commuting, but since they will still receive students from each other
        // Kortrijk will only receive students from Kortrijik
        auto brasschaat = *geoGrid.begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto schoten = *(geoGrid.begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto kortrijk = *(geoGrid.begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        geoGrid.Finalize();
        workplacePopulator.Apply(geoGrid, config);

        for (const Person& p : *geoGrid.GetPopulation()) {
                EXPECT_EQ(0, p.GetPoolId(Id::Workplace));
        }
}

TEST(WorkplacePopulatorTest, NoCommuting)
{
        auto pop = Population::Create();
        SetupGeoGrid(3, 100, 3, 33, 3, pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        auto               rnMan = RnMan(RnInfo{});
        WorkplacePopulator workplacePopulator(rnMan);
        GeoGridConfig      config{};
        unsigned int       contactCenterCounter   = 1;
        config.input.fraction_workplace_commuters = 0;
        config.input.particpation_workplace       = 1;
        config.input.participation_college        = 0.5;

        // Brasschaat and Schoten are close to each other
        // There is no commuting, but since they will still receive students from each other
        // Kortrijk will only receive students from Kortrijik
        auto brasschaat = *geoGrid.begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto workBra1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workBra1->SetupPools(config, pop.get());
        brasschaat->AddCenter(workBra1);
        auto workBra2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workBra2->SetupPools(config, pop.get());
        brasschaat->AddCenter(workBra2);
        auto schoten = *(geoGrid.begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto workScho1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workScho1->SetupPools(config, pop.get());
        schoten->AddCenter(workScho1);
        auto workScho2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workScho2->SetupPools(config, pop.get());
        schoten->AddCenter(workScho2);
        auto kortrijk = *(geoGrid.begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        auto workKor1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workKor1->SetupPools(config, pop.get());
        kortrijk->AddCenter(workKor1);
        auto workKor2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workKor2->SetupPools(config, pop.get());
        kortrijk->AddCenter(workKor2);

        geoGrid.Finalize();
        workplacePopulator.Apply(geoGrid, config);

        const auto pwc = GeoGridConfig().pools.pools_per_workplace;

        // Assert that persons of Schoten only go to Schoten or Brasschaat
        for (const auto& hCenter : schoten->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 4 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 4 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Brasschaat only go to Schoten or Brasschaat
        for (const auto& hCenter : brasschaat->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 4 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 4 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Kortijk
        for (const auto& hCenter : kortrijk->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 4 * pwc && workId <= 6 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 4 * pwc && workId <= 6 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }
}

TEST(WorkplacePopulatorTest, OnlyCommuting)
{
        auto pop = Population::Create();
        SetupGeoGrid(3, 100, 3, 33, 3, pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        auto               rnMan = RnMan(RnInfo{});
        WorkplacePopulator workplacePopulator(rnMan);
        GeoGridConfig      config{};
        unsigned int       contactCenterCounter   = 1;
        config.input.fraction_workplace_commuters = 0;
        config.input.fraction_workplace_commuters = 1;
        config.input.fraction_college_commuters   = 0;
        config.popInfo.popcount_workplace         = 1;
        config.input.particpation_workplace       = 1;
        config.input.participation_college        = 0.5;

        // only commuting
        auto schoten = *(geoGrid.begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));

        auto workScho1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workScho1->SetupPools(config, pop.get());
        schoten->AddCenter(workScho1);
        auto workScho2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workScho2->SetupPools(config, pop.get());
        schoten->AddCenter(workScho2);
        auto kortrijk = *(geoGrid.begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        auto workKor1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workKor1->SetupPools(config, pop.get());
        kortrijk->AddCenter(workKor1);
        auto workKor2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workKor2->SetupPools(config, pop.get());
        kortrijk->AddCenter(workKor2);

        schoten->AddOutgoingCommute(kortrijk, 0.5);
        kortrijk->AddIncomingCommute(schoten, 0.5);
        kortrijk->AddOutgoingCommute(schoten, 0.5);
        schoten->AddIncomingCommute(kortrijk, 0.5);

        geoGrid.Finalize();
        workplacePopulator.Apply(geoGrid, config);

        const auto pwc = GeoGridConfig().pools.pools_per_workplace;

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& hCenter : schoten->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 2 * pwc && workId <= 4 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 2 * pwc && workId <= 4 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Schoten
        for (const auto& hCenter : kortrijk->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 2 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 2 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }
}

TEST(WorkplacePopulatorTest, OnlyCommutingButNoCommutingAvaiable)
{

        auto pop = Population::Create();
        SetupGeoGrid(3, 100, 3, 33, 3, pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        auto               rnMan = RnMan{RnInfo{}};
        WorkplacePopulator workplacePopulator(rnMan);
        GeoGridConfig      config{};
        unsigned int       contactCenterCounter   = 1;
        config.input.fraction_workplace_commuters = 0;
        config.input.fraction_workplace_commuters = 1;
        config.input.fraction_college_commuters   = 0;
        config.popInfo.popcount_workplace         = 1;
        config.input.particpation_workplace       = 1;
        config.input.participation_college        = 0.5;

        auto brasschaat = *geoGrid.begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto workBra1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workBra1->SetupPools(config, pop.get());
        brasschaat->AddCenter(workBra1);
        auto workBra2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workBra2->SetupPools(config, pop.get());
        brasschaat->AddCenter(workBra2);
        auto schoten = *(geoGrid.begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto workScho1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workScho1->SetupPools(config, pop.get());
        schoten->AddCenter(workScho1);
        auto workScho2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workScho2->SetupPools(config, pop.get());
        schoten->AddCenter(workScho2);
        auto kortrijk = *(geoGrid.begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        auto workKor1 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workKor1->SetupPools(config, pop.get());
        kortrijk->AddCenter(workKor1);
        auto workKor2 = make_shared<WorkplaceCenter>(contactCenterCounter++);
        workKor2->SetupPools(config, pop.get());
        kortrijk->AddCenter(workKor2);

        // test case is only commuting but between nobody is commuting from or to Brasschaat
        schoten->AddOutgoingCommute(kortrijk, 0.5);
        kortrijk->AddIncomingCommute(schoten, 0.5);
        kortrijk->AddOutgoingCommute(schoten, 0.5);
        schoten->AddIncomingCommute(kortrijk, 0.5);

        geoGrid.Finalize();
        workplacePopulator.Apply(geoGrid, config);

        const auto pwc = GeoGridConfig().pools.pools_per_workplace;

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& hCenter : schoten->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 4 * pwc && workId <= 6 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 4 * pwc && workId <= 6 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Brasschaat only go to Brasschaat or Schoten
        for (const auto& hCenter : brasschaat->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 4 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 4 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Schoten
        for (const auto& hCenter : kortrijk->RefCenters(Id::Household)) {
                for (auto p : *(*hCenter)[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 2 * pwc && workId <= 4 * pwc);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 2 * pwc && workId <= 4 * pwc) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }
}

} // namespace
