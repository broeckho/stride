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

#include "geopop/populators/K12SchoolPopulator.h"

#include "contact/AgeBrackets.h"
#include "createGeogrid.h"
#include "geopop/GeoGrid.h"
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

TEST(K12SchoolPopulatorTest, NoPopulation)
{
        auto rnManager = RnMan(RnMan::Info{});
        auto pop       = Population::Create();
        auto geoGrid   = make_shared<GeoGrid>(pop.get());

        geoGrid->AddLocation(make_shared<Location>(0, 0, 0));
        K12SchoolPopulator k12SchoolPopulator(rnManager);
        GeoGridConfig      config{};

        geoGrid->Finalize();

        EXPECT_NO_THROW(k12SchoolPopulator.Apply(geoGrid, config));
}

TEST(K12SchoolPopulatorTest, OneLocationTest)
{
        auto rnManager = RnMan(RnMan::Info{});
        auto pop       = Population::Create();
        auto geoGrid   = CreateGeoGrid(1, 300, 5, 100, 3, pop.get());

        K12SchoolPopulator k12SchoolPopulator(rnManager);
        GeoGridConfig      config{};

        geoGrid->Finalize();
        k12SchoolPopulator.Apply(geoGrid, config);

        auto location   = *geoGrid->begin();
        auto k12Schools = location->GetContactCentersOfType<K12School>();

        EXPECT_EQ(5, k12Schools.size());

        map<int, int> usedCapacity{
            {1, 1},   {2, 1},   {3, 0},   {4, 0},   {5, 0},   {6, 0},   {7, 1},   {8, 1},   {9, 2},   {10, 1},
            {11, 1},  {12, 1},  {13, 0},  {14, 1},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {20, 0},
            {21, 1},  {22, 0},  {23, 0},  {24, 1},  {25, 0},  {26, 1},  {27, 0},  {28, 1},  {29, 0},  {30, 0},
            {31, 0},  {32, 0},  {33, 2},  {34, 0},  {35, 0},  {36, 0},  {37, 0},  {38, 0},  {39, 0},  {40, 0},
            {41, 2},  {42, 0},  {43, 0},  {44, 0},  {45, 1},  {46, 0},  {47, 0},  {48, 0},  {49, 1},  {50, 0},
            {51, 0},  {52, 1},  {53, 0},  {54, 0},  {55, 0},  {56, 0},  {57, 1},  {58, 1},  {59, 0},  {60, 1},
            {61, 0},  {62, 1},  {63, 0},  {64, 0},  {65, 0},  {66, 0},  {67, 0},  {68, 0},  {69, 0},  {70, 1},
            {71, 0},  {72, 1},  {73, 1},  {74, 0},  {75, 0},  {76, 0},  {77, 0},  {78, 1},  {79, 1},  {80, 0},
            {81, 1},  {82, 2},  {83, 0},  {84, 0},  {85, 0},  {86, 1},  {87, 0},  {88, 1},  {89, 0},  {90, 0},
            {91, 0},  {92, 1},  {93, 0},  {94, 0},  {95, 1},  {96, 0},  {97, 0},  {98, 0},  {99, 1},  {100, 0},
            {101, 0}, {102, 0}, {103, 1}, {104, 0}, {105, 0}, {106, 1}, {107, 0}, {108, 0}, {109, 1}, {110, 0},
            {111, 1}, {112, 0}, {113, 1}, {114, 0}, {115, 0}, {116, 0}, {117, 0}, {118, 0}, {119, 0}, {120, 1},
            {121, 0}, {122, 1}, {123, 0}, {124, 0}, {125, 0}};

        for (auto& k12School : k12Schools) {
                EXPECT_EQ(25, k12School->GetPools().size());
                for (auto& pool : k12School->GetPools()) {
                        EXPECT_EQ(usedCapacity[pool->GetId()], pool->GetSize());
                        for (Person* person : *pool) {
                                EXPECT_LE(person->GetAge(), 18);
                                EXPECT_GE(person->GetAge(), 6);
                        }
                }
        }

        map<int, int> persons{
            {0, 14},    {1, 0},     {2, 0},     {3, 0},     {4, 0},     {5, 0},    {6, 0},    {7, 0},    {8, 0},
            {9, 0},     {10, 0},    {11, 106},  {12, 0},    {13, 0},    {14, 24},  {15, 0},   {16, 0},   {17, 0},
            {18, 0},    {19, 0},    {20, 0},    {21, 0},    {22, 0},    {23, 0},   {24, 0},   {25, 0},   {26, 0},
            {27, 0},    {28, 60},   {29, 0},    {30, 0},    {31, 0},    {32, 81},  {33, 0},   {34, 0},   {35, 0},
            {36, 0},    {37, 0},    {38, 0},    {39, 0},    {40, 0},    {41, 0},   {42, 0},   {43, 88},  {44, 0},
            {45, 0},    {46, 0},    {47, 0},    {48, 0},    {49, 92},   {50, 0},   {51, 0},   {52, 0},   {53, 0},
            {54, 0},    {55, 0},    {56, 26},   {57, 0},    {58, 0},    {59, 45},  {60, 9},   {61, 2},   {62, 0},
            {63, 41},   {64, 0},    {65, 0},    {66, 0},    {67, 0},    {68, 122}, {69, 0},   {70, 0},   {71, 0},
            {72, 0},    {73, 0},    {74, 0},    {75, 73},   {76, 0},    {77, 0},   {78, 0},   {79, 0},   {80, 0},
            {81, 0},    {82, 0},    {83, 0},    {84, 0},    {85, 0},    {86, 0},   {87, 33},  {88, 0},   {89, 0},
            {90, 0},    {91, 0},    {92, 0},    {93, 0},    {94, 0},    {95, 0},   {96, 0},   {97, 0},   {98, 0},
            {99, 0},    {100, 0},   {101, 8},   {102, 113}, {103, 0},   {104, 0},  {105, 0},  {106, 0},  {107, 0},
            {108, 58},  {109, 0},   {110, 0},   {111, 0},   {112, 0},   {113, 0},  {114, 0},  {115, 0},  {116, 0},
            {117, 0},   {118, 109}, {119, 0},   {120, 0},   {121, 0},   {122, 0},  {123, 0},  {124, 0},  {125, 0},
            {126, 0},   {127, 0},   {128, 0},   {129, 0},   {130, 0},   {131, 0},  {132, 12}, {133, 0},  {134, 0},
            {135, 0},   {136, 0},   {137, 0},   {138, 0},   {139, 78},  {140, 0},  {141, 0},  {142, 0},  {143, 0},
            {144, 0},   {145, 0},   {146, 0},   {147, 0},   {148, 0},   {149, 0},  {150, 0},  {151, 0},  {152, 0},
            {153, 57},  {154, 0},   {155, 0},   {156, 0},   {157, 82},  {158, 0},  {159, 0},  {160, 0},  {161, 0},
            {162, 0},   {163, 0},   {164, 103}, {165, 0},   {166, 0},   {167, 49}, {168, 0},  {169, 0},  {170, 82},
            {171, 0},   {172, 0},   {173, 95},  {174, 1},   {175, 0},   {176, 0},  {177, 0},  {178, 0},  {179, 0},
            {180, 0},   {181, 0},   {182, 0},   {183, 62},  {184, 0},   {185, 41}, {186, 0},  {187, 0},  {188, 99},
            {189, 0},   {190, 0},   {191, 0},   {192, 79},  {193, 0},   {194, 0},  {195, 0},  {196, 0},  {197, 0},
            {198, 0},   {199, 0},   {200, 0},   {201, 0},   {202, 0},   {203, 0},  {204, 33}, {205, 52}, {206, 0},
            {207, 0},   {208, 10},  {209, 0},   {210, 11},  {211, 7},   {212, 0},  {213, 0},  {214, 0},  {215, 0},
            {216, 0},   {217, 0},   {218, 0},   {219, 0},   {220, 0},   {221, 0},  {222, 0},  {223, 0},  {224, 0},
            {225, 0},   {226, 0},   {227, 0},   {228, 0},   {229, 0},   {230, 72}, {231, 0},  {232, 0},  {233, 0},
            {234, 0},   {235, 0},   {236, 0},   {237, 0},   {238, 0},   {239, 0},  {240, 0},  {241, 0},  {242, 0},
            {243, 0},   {244, 0},   {245, 0},   {246, 0},   {247, 120}, {248, 28}, {249, 0},  {250, 0},  {251, 0},
            {252, 86},  {253, 0},   {254, 0},   {255, 0},   {256, 0},   {257, 70}, {258, 0},  {259, 0},  {260, 0},
            {261, 0},   {262, 0},   {263, 9},   {264, 0},   {265, 0},   {266, 0},  {267, 0},  {268, 0},  {269, 0},
            {270, 0},   {271, 0},   {272, 0},   {273, 0},   {274, 0},   {275, 0},  {276, 0},  {277, 0},  {278, 0},
            {279, 0},   {280, 0},   {281, 0},   {282, 0},   {283, 21},  {284, 0},  {285, 0},  {286, 0},  {287, 0},
            {288, 0},   {289, 0},   {290, 0},   {291, 0},   {292, 0},   {293, 0},  {294, 0},  {295, 0},  {296, 0},
            {297, 111}, {298, 0},   {299, 0}};

        for (const auto& person : *geoGrid->GetPopulation()) {
                EXPECT_EQ(persons[person.GetId()], person.GetPoolId(Id::K12School));
        }
}

TEST(K12SchoolPopulatorTest, TwoLocationTest)
{
        auto rnManager = RnMan{};
        auto pop       = Population::Create();
        auto geoGrid   = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());

        K12SchoolPopulator k12SchoolPopulator(rnManager);
        GeoGridConfig      config{};

        // Brasschaat and Schoten are close to each oter and will both have students from both
        // Kortrijk will only have students going to Kortrijk
        auto brasschaat = *geoGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto schoten = *(geoGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto kortrijk = *(geoGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        geoGrid->Finalize();
        k12SchoolPopulator.Apply(geoGrid, config);

        auto k12Schools1 = brasschaat->GetContactCentersOfType<K12School>();
        auto k12Schools2 = schoten->GetContactCentersOfType<K12School>();
        auto k12Schools3 = kortrijk->GetContactCentersOfType<K12School>();

        EXPECT_EQ(3, k12Schools1.size());
        EXPECT_EQ(3, k12Schools2.size());
        EXPECT_EQ(3, k12Schools3.size());

        map<int, int> persons{
            {0, 125},   {1, 0},     {2, 0},     {3, 0},     {4, 0},     {5, 0},     {6, 0},     {7, 0},     {8, 0},
            {9, 0},     {10, 0},    {11, 52},   {12, 0},    {13, 0},    {14, 136},  {15, 0},    {16, 0},    {17, 0},
            {18, 0},    {19, 0},    {20, 0},    {21, 0},    {22, 0},    {23, 0},    {24, 0},    {25, 0},    {26, 0},
            {27, 0},    {28, 180},  {29, 0},    {30, 0},    {31, 0},    {32, 22},   {33, 0},    {34, 0},    {35, 0},
            {36, 0},    {37, 0},    {38, 0},    {39, 0},    {40, 0},    {41, 0},    {42, 0},    {43, 30},   {44, 0},
            {45, 0},    {46, 0},    {47, 0},    {48, 0},    {49, 35},   {50, 0},    {51, 0},    {52, 0},    {53, 0},
            {54, 0},    {55, 0},    {56, 139},  {57, 0},    {58, 0},    {59, 162},  {60, 118},  {61, 110},  {62, 0},
            {63, 157},  {64, 0},    {65, 0},    {66, 0},    {67, 0},    {68, 71},   {69, 0},    {70, 0},    {71, 0},
            {72, 0},    {73, 0},    {74, 0},    {75, 13},   {76, 0},    {77, 0},    {78, 0},    {79, 0},    {80, 0},
            {81, 0},    {82, 0},    {83, 0},    {84, 0},    {85, 0},    {86, 0},    {87, 148},  {88, 0},    {89, 0},
            {90, 0},    {91, 0},    {92, 0},    {93, 0},    {94, 0},    {95, 0},    {96, 0},    {97, 0},    {98, 0},
            {99, 0},    {100, 0},   {101, 118}, {102, 60},  {103, 0},   {104, 0},   {105, 0},   {106, 0},   {107, 0},
            {108, 178}, {109, 0},   {110, 0},   {111, 0},   {112, 0},   {113, 0},   {114, 0},   {115, 0},   {116, 0},
            {117, 0},   {118, 56},  {119, 0},   {120, 0},   {121, 0},   {122, 0},   {123, 0},   {124, 0},   {125, 0},
            {126, 0},   {127, 0},   {128, 0},   {129, 0},   {130, 0},   {131, 0},   {132, 123}, {133, 0},   {134, 0},
            {135, 0},   {136, 0},   {137, 0},   {138, 0},   {139, 19},  {140, 0},   {141, 0},   {142, 0},   {143, 0},
            {144, 0},   {145, 0},   {146, 0},   {147, 0},   {148, 0},   {149, 0},   {150, 0},   {151, 0},   {152, 0},
            {153, 176}, {154, 0},   {155, 0},   {156, 0},   {157, 24},  {158, 0},   {159, 0},   {160, 0},   {161, 0},
            {162, 0},   {163, 0},   {164, 49},  {165, 0},   {166, 0},   {167, 167}, {168, 0},   {169, 0},   {170, 23},
            {171, 0},   {172, 0},   {173, 39},  {174, 109}, {175, 0},   {176, 0},   {177, 0},   {178, 0},   {179, 0},
            {180, 0},   {181, 0},   {182, 0},   {183, 183}, {184, 0},   {185, 158}, {186, 0},   {187, 0},   {188, 44},
            {189, 0},   {190, 0},   {191, 0},   {192, 20},  {193, 0},   {194, 0},   {195, 0},   {196, 0},   {197, 0},
            {198, 0},   {199, 0},   {200, 0},   {201, 0},   {202, 0},   {203, 0},   {204, 236}, {205, 247}, {206, 0},
            {207, 0},   {208, 222}, {209, 0},   {210, 223}, {211, 220}, {212, 0},   {213, 0},   {214, 0},   {215, 0},
            {216, 0},   {217, 0},   {218, 0},   {219, 0},   {220, 0},   {221, 0},   {222, 0},   {223, 0},   {224, 0},
            {225, 0},   {226, 0},   {227, 0},   {228, 0},   {229, 0},   {230, 259}, {231, 0},   {232, 0},   {233, 0},
            {234, 0},   {235, 0},   {236, 0},   {237, 0},   {238, 0},   {239, 0},   {240, 0},   {241, 0},   {242, 0},
            {243, 0},   {244, 0},   {245, 0},   {246, 0},   {247, 288}, {248, 233}, {249, 0},   {250, 0},   {251, 0},
            {252, 268}, {253, 0},   {254, 0},   {255, 0},   {256, 0},   {257, 258}, {258, 0},   {259, 0},   {260, 0},
            {261, 0},   {262, 0},   {263, 221}, {264, 0},   {265, 0},   {266, 0},   {267, 0},   {268, 0},   {269, 0},
            {270, 0},   {271, 0},   {272, 0},   {273, 0},   {274, 0},   {275, 0},   {276, 0},   {277, 0},   {278, 0},
            {279, 0},   {280, 0},   {281, 0},   {282, 0},   {283, 229}, {284, 0},   {285, 0},   {286, 0},   {287, 0},
            {288, 0},   {289, 0},   {290, 0},   {291, 0},   {292, 0},   {293, 0},   {294, 0},   {295, 0},   {296, 0}};

        for (const auto& person : *geoGrid->GetPopulation()) {
                EXPECT_EQ(persons[person.GetId()], person.GetPoolId(Id::K12School));
        }

        for (const auto& household : kortrijk->GetContactCentersOfType<Household>()) {
                for (const auto& p : *household->GetPools()[0]) {
                        const auto k12Id = p->GetPoolId(Id::K12School);
                        if (AgeBrackets::K12School::HasAge(p->GetAge())) {
                                EXPECT_TRUE(k12Id >= 217 && k12Id <= 291);
                        } else {
                                EXPECT_EQ(0, k12Id);
                        }
                }
        }
}

} // namespace
