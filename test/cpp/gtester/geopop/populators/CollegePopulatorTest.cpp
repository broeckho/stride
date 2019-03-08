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

#include "geopop/populators/CollegePopulator.h"

#include "contact/AgeBrackets.h"
#include "createGeogrid.h"
#include "geopop/College.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"
#include "geopop/K12School.h"
#include "geopop/Location.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>
#include <map>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

TEST(CollegePopulatorTest, NoPopulation)
{
        RnMan rnManager{}; // Default random number manager.
        auto  pop     = Population::Create();
        auto  geoGrid = make_shared<GeoGrid>(pop.get());
        geoGrid->AddLocation(make_shared<Location>(0, 0, 0));
        geoGrid->Finalize();
        CollegePopulator populator(rnManager);
        GeoGridConfig    config{};

        EXPECT_NO_THROW(populator.Apply(geoGrid, config));
}

TEST(CollegePopulatorTest, NoStudents)
{
        RnMan            rnManager(RnMan::Info{}); // Default random number manager.
        auto             pop     = Population::Create();
        auto             geoGrid = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());
        CollegePopulator populator(rnManager);
        GeoGridConfig    config{};
        unsigned int     contactCenterCounter   = 1;
        config.input.fraction_college_commuters = 0;
        config.input.participation_college      = 0;

        // Brasschaat and Schoten are close to each other
        // There is no commuting, but since they will still receive students from each other
        // Kortrijk will only receive students from Kortrijk
        auto brasschaat = *geoGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto collegeBra = make_shared<College>(contactCenterCounter++);
        collegeBra->Fill(config, geoGrid);
        brasschaat->AddContactCenter(collegeBra);

        auto schoten = *(geoGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto collegeScho = make_shared<College>(contactCenterCounter++);
        collegeScho->Fill(config, geoGrid);
        schoten->AddContactCenter(collegeScho);

        auto kortrijk = *(geoGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        auto collegeKort = make_shared<College>(contactCenterCounter++);
        collegeKort->Fill(config, geoGrid);
        kortrijk->AddContactCenter(collegeKort);

        geoGrid->Finalize();
        populator.Apply(geoGrid, config);

        for (const auto& person : *geoGrid->GetPopulation()) {
                EXPECT_EQ(0, person.GetPoolId(Id::College));
        }
}

TEST(CollegePopulatorTest, NotCommuting)
{
        RnMan            rnManager(RnMan::Info{}); // Default random number manager.
        auto             pop     = Population::Create();
        auto             geoGrid = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());
        CollegePopulator populator(rnManager);
        GeoGridConfig    config{};
        unsigned int     contactCenterCounter   = 1;
        config.input.fraction_college_commuters = 0;
        config.input.participation_college      = 1;

        // Brasschaat and Schoten are close to each other
        // There is no commuting, but since they will still receive students from each other
        // Kortrijk will only receive students from Kortrijik
        auto brasschaat = *geoGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto collegeBra = make_shared<College>(contactCenterCounter++);
        collegeBra->Fill(config, geoGrid);
        brasschaat->AddContactCenter(collegeBra);

        auto schoten = *(geoGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto collegeScho = make_shared<College>(contactCenterCounter++);
        collegeScho->Fill(config, geoGrid);
        schoten->AddContactCenter(collegeScho);

        auto kortrijk = *(geoGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        auto collegeKort = make_shared<College>(contactCenterCounter++);
        collegeKort->Fill(config, geoGrid);
        kortrijk->AddContactCenter(collegeKort);

        geoGrid->Finalize();
        populator.Apply(geoGrid, config);

        map<int, int> persons{
            {0, 0},     {1, 0},     {2, 0},     {3, 0},     {4, 0},     {5, 0},     {6, 0},     {7, 0},    {8, 0},
            {9, 0},     {10, 0},    {11, 0},    {12, 0},    {13, 0},    {14, 0},    {15, 0},    {16, 338}, {17, 0},
            {18, 0},    {19, 0},    {20, 0},    {21, 0},    {22, 0},    {23, 0},    {24, 0},    {25, 0},   {26, 0},
            {27, 364},  {28, 0},    {29, 0},    {30, 0},    {31, 0},    {32, 0},    {33, 0},    {34, 0},   {35, 0},
            {36, 332},  {37, 353},  {38, 0},    {39, 347},  {40, 0},    {41, 357},  {42, 0},    {43, 0},   {44, 0},
            {45, 0},    {46, 0},    {47, 0},    {48, 0},    {49, 0},    {50, 328},  {51, 0},    {52, 0},   {53, 0},
            {54, 0},    {55, 0},    {56, 0},    {57, 0},    {58, 0},    {59, 0},    {60, 0},    {61, 0},   {62, 0},
            {63, 0},    {64, 0},    {65, 347},  {66, 0},    {67, 0},    {68, 0},    {69, 0},    {70, 363}, {71, 0},
            {72, 0},    {73, 0},    {74, 0},    {75, 0},    {76, 0},    {77, 0},    {78, 0},    {79, 0},   {80, 0},
            {81, 0},    {82, 0},    {83, 0},    {84, 0},    {85, 0},    {86, 348},  {87, 0},    {88, 0},   {89, 0},
            {90, 0},    {91, 0},    {92, 0},    {93, 0},    {94, 0},    {95, 0},    {96, 0},    {97, 0},   {98, 362},
            {99, 0},    {100, 0},   {101, 0},   {102, 0},   {103, 0},   {104, 337}, {105, 0},   {106, 0},  {107, 0},
            {108, 0},   {109, 0},   {110, 0},   {111, 0},   {112, 0},   {113, 0},   {114, 0},   {115, 0},  {116, 0},
            {117, 0},   {118, 0},   {119, 0},   {120, 0},   {121, 0},   {122, 0},   {123, 0},   {124, 0},  {125, 0},
            {126, 0},   {127, 0},   {128, 0},   {129, 0},   {130, 0},   {131, 331}, {132, 0},   {133, 0},  {134, 0},
            {135, 0},   {136, 0},   {137, 0},   {138, 0},   {139, 0},   {140, 0},   {141, 0},   {142, 0},  {143, 0},
            {144, 0},   {145, 0},   {146, 0},   {147, 0},   {148, 0},   {149, 0},   {150, 0},   {151, 0},  {152, 0},
            {153, 0},   {154, 0},   {155, 0},   {156, 0},   {157, 0},   {158, 0},   {159, 0},   {160, 0},  {161, 0},
            {162, 345}, {163, 358}, {164, 0},   {165, 0},   {166, 0},   {167, 0},   {168, 0},   {169, 0},  {170, 0},
            {171, 0},   {172, 0},   {173, 0},   {174, 0},   {175, 330}, {176, 0},   {177, 0},   {178, 0},  {179, 0},
            {180, 0},   {181, 0},   {182, 0},   {183, 0},   {184, 0},   {185, 0},   {186, 0},   {187, 0},  {188, 0},
            {189, 0},   {190, 0},   {191, 0},   {192, 0},   {193, 0},   {194, 361}, {195, 0},   {196, 0},  {197, 0},
            {198, 366}, {199, 0},   {200, 376}, {201, 0},   {202, 0},   {203, 0},   {204, 0},   {205, 0},  {206, 369},
            {207, 0},   {208, 0},   {209, 0},   {210, 0},   {211, 0},   {212, 0},   {213, 0},   {214, 0},  {215, 0},
            {216, 0},   {217, 0},   {218, 376}, {219, 368}, {220, 0},   {221, 0},   {222, 0},   {223, 0},  {224, 0},
            {225, 378}, {226, 0},   {227, 0},   {228, 0},   {229, 0},   {230, 0},   {231, 0},   {232, 0},  {233, 0},
            {234, 0},   {235, 0},   {236, 384}, {237, 0},   {238, 0},   {239, 0},   {240, 0},   {241, 0},  {242, 0},
            {243, 0},   {244, 369}, {245, 0},   {246, 383}, {247, 0},   {248, 0},   {249, 0},   {250, 0},  {251, 0},
            {252, 0},   {253, 0},   {254, 0},   {255, 0},   {256, 375}, {257, 0},   {258, 0},   {259, 0},  {260, 0},
            {261, 0},   {262, 0},   {263, 0},   {264, 0},   {265, 0},   {266, 0},   {267, 0},   {268, 0},  {269, 0},
            {270, 371}, {271, 0},   {272, 367}, {273, 0},   {274, 0},   {275, 0},   {276, 370}, {277, 0},  {278, 0},
            {279, 0},   {280, 0},   {281, 0},   {282, 0},   {283, 0},   {284, 0},   {285, 377}, {286, 0},  {287, 0},
            {288, 369}, {289, 0},   {290, 376}, {291, 0},   {292, 0},   {293, 0},   {294, 0},   {295, 0},  {296, 0}};

        for (const auto& p : *geoGrid->GetPopulation()) {
                EXPECT_EQ(persons[p.GetId()], p.GetPoolId(Id::College));
                if (AgeBrackets::College::HasAge(p.GetAge())) {
                        EXPECT_NE(0, p.GetPoolId(Id::College));
                } else {
                        EXPECT_EQ(0, p.GetPoolId(Id::College));
                }
        }

        // Assert that persons of Schoten only go to Schoten or Brasschaat
        for (const auto& household : schoten->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 325 && p->GetPoolId(Id::College) <= 364);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }

        // Assert that persons of Schoten only go to Schoten or Brasschaat
        for (const auto& household : brasschaat->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 325 && p->GetPoolId(Id::College) <= 364);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Kortijk
        for (const auto& household : kortrijk->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 365 && p->GetPoolId(Id::College) <= 384);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }
}

TEST(CollegePopulatorTest, OnlyCommuting)
{
        RnMan rnManager(RnMan::Info{}); // Default random number manager.

        CollegePopulator populator(rnManager);
        GeoGridConfig    config{};
        unsigned int     contactCenterCounter   = 1;
        config.input.fraction_college_commuters = 1;
        config.input.participation_college      = 1;

        auto pop      = Population::Create();
        auto geoGrid  = CreateGeoGrid(2, 100, 3, 50, 3, pop.get());
        auto location = *geoGrid->begin();

        // only commuting
        auto schoten = *(geoGrid->begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto collegeScho = make_shared<College>(contactCenterCounter++);
        collegeScho->Fill(config, geoGrid);
        schoten->AddContactCenter(collegeScho);

        auto kortrijk = *(geoGrid->begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        auto collegeKort = make_shared<College>(contactCenterCounter++);
        collegeKort->Fill(config, geoGrid);
        kortrijk->AddContactCenter(collegeKort);

        schoten->AddOutgoingCommutingLocation(kortrijk, 0.5);
        kortrijk->AddIncomingCommutingLocation(schoten, 0.5);
        kortrijk->AddOutgoingCommutingLocation(schoten, 0.5);
        schoten->AddIncomingCommutingLocation(kortrijk, 0.5);

        geoGrid->Finalize();

        populator.Apply(geoGrid, config);

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& household : schoten->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 271 && p->GetPoolId(Id::College) <= 290);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Schoten
        for (const auto& household : kortrijk->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 251 && p->GetPoolId(Id::College) <= 270);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }
}

TEST(CollegePopulatorTest, OnlyCommutingButNoCommutingAvaiable)
{
        RnMan            rnManager(RnMan::Info{}); // Default random number manager.
        auto             pop     = Population::Create();
        auto             geoGrid = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());
        CollegePopulator populator(rnManager);
        GeoGridConfig    config{};
        unsigned int     contactCenterCounter   = 1;
        config.input.fraction_college_commuters = 1;
        config.input.participation_college      = 1;

        auto brasschaat = *geoGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto collegeBra = make_shared<College>(contactCenterCounter++);
        collegeBra->Fill(config, geoGrid);
        brasschaat->AddContactCenter(collegeBra);

        auto schoten = *(geoGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto collegeScho = make_shared<College>(contactCenterCounter++);
        collegeScho->Fill(config, geoGrid);
        schoten->AddContactCenter(collegeScho);

        auto kortrijk = *(geoGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        auto collegeKort = make_shared<College>(contactCenterCounter++);
        collegeKort->Fill(config, geoGrid);
        kortrijk->AddContactCenter(collegeKort);

        // test case is only commuting but between nobody is commuting from or to Brasschaat
        schoten->AddOutgoingCommutingLocation(kortrijk, 0.5);
        kortrijk->AddIncomingCommutingLocation(schoten, 0.5);

        kortrijk->AddOutgoingCommutingLocation(schoten, 0.5);
        schoten->AddIncomingCommutingLocation(kortrijk, 0.5);

        geoGrid->Finalize();

        populator.Apply(geoGrid, config);

        // Assert that persons of Schoten only commute to Kortrijk
        for (const auto& household : schoten->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 365 && p->GetPoolId(Id::College) <= 384);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }

        // Assert that persons of Brasschaat only commute to Brasschaat or Schoten
        for (const auto& household : brasschaat->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 325 && p->GetPoolId(Id::College) <= 364);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }

        // Assert that persons of Kortrijk only commute to Schoten
        for (const auto& household : kortrijk->GetContactCentersOfType<Household>()) {
                for (auto p : *household->GetPools()[0]) {
                        if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(p->GetPoolId(Id::College) >= 345 && p->GetPoolId(Id::College) <= 364);
                        } else {
                                EXPECT_EQ(0, p->GetPoolId(Id::College));
                        }
                }
        }
}

} // namespace
