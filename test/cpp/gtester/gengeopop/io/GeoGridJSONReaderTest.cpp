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

#include <fstream>
#include <gtest/gtest.h>
#include <iomanip>
#include <memory>

#include <gengeopop/ContactCenter.h>
#include <gengeopop/io/GeoGridJSONReader.h>
#include <util/Exception.h>
#include <util/FileSys.h>

using namespace gengeopop;

using boost::geometry::get;

namespace {

std::shared_ptr<GeoGrid> getGeoGridForFile(std::string filename, stride::Population* pop)
{
        auto file = std::make_unique<std::ifstream>();
        file->open(stride::util::FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/" + filename);

        GeoGridJSONReader geoGridJSONReader(std::move(file), pop);
        auto              geoGrid = geoGridJSONReader.Read();
        return geoGrid;
}

TEST(GeoGridJSONReaderTest, locationsTest)
{
        auto pop     = stride::Population::Create();
        auto geoGrid = getGeoGridForFile("test0.json", pop.get());

        std::map<unsigned int, std::shared_ptr<Location>> locations;

        locations[geoGrid->Get(0)->GetID()] = geoGrid->Get(0);
        locations[geoGrid->Get(1)->GetID()] = geoGrid->Get(1);
        locations[geoGrid->Get(2)->GetID()] = geoGrid->Get(2);

        auto location1 = locations[1];
        auto location2 = locations[2];
        auto location3 = locations[3];

        EXPECT_EQ(location1->GetID(), 1);
        EXPECT_EQ(location1->GetName(), "Bavikhove");
        EXPECT_EQ(location1->GetProvince(), 4);
        EXPECT_EQ(location1->GetPopulation(), 2500);
        EXPECT_EQ(get<0>(location1->GetCoordinate()), 0);
        EXPECT_EQ(get<1>(location1->GetCoordinate()), 0);

        EXPECT_EQ(location2->GetID(), 2);
        EXPECT_EQ(location2->GetName(), "Gent");
        EXPECT_EQ(location2->GetProvince(), 3);
        EXPECT_EQ(location2->GetPopulation(), 5000);
        EXPECT_EQ(get<0>(location2->GetCoordinate()), 0);
        EXPECT_EQ(get<1>(location2->GetCoordinate()), 0);

        EXPECT_EQ(location3->GetID(), 3);
        EXPECT_EQ(location3->GetName(), "Mons");
        EXPECT_EQ(location3->GetProvince(), 2);
        EXPECT_EQ(location3->GetPopulation(), 2500);
        EXPECT_EQ(get<0>(location3->GetCoordinate()), 0);
        EXPECT_EQ(get<1>(location3->GetCoordinate()), 0);
}

TEST(GeoGridJSONReaderTest, commutesTest)
{
        auto pop     = stride::Population::Create();
        auto geoGrid = getGeoGridForFile("test7.json", pop.get());

        std::map<unsigned int, std::shared_ptr<Location>> locations;

        locations[geoGrid->Get(0)->GetID()] = geoGrid->Get(0);
        locations[geoGrid->Get(1)->GetID()] = geoGrid->Get(1);
        locations[geoGrid->Get(2)->GetID()] = geoGrid->Get(2);

        auto location1 = locations[1];
        auto location2 = locations[2];
        auto location3 = locations[3];

        auto sortLoc = [](std::vector<std::pair<Location*, double>> loc) {
                std::sort(std::begin(loc), std::end(loc),
                          [](const std::pair<Location*, double>& a, const std::pair<Location*, double>& b) {
                                  return a.first->GetID() < b.first->GetID();
                          });
                return loc;
        };

        {
                auto commuting_in  = sortLoc(location1->GetIncomingCommuningCities());
                auto commuting_out = sortLoc(location1->GetOutgoingCommuningCities());
                EXPECT_EQ(commuting_in.size(), 1);
                EXPECT_EQ(commuting_out.size(), 2);

                EXPECT_EQ(commuting_in[0].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.75);

                EXPECT_EQ(commuting_out[0].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_out[0].second, 0.50);
                EXPECT_EQ(commuting_out[1].first->GetID(), 3);
                EXPECT_DOUBLE_EQ(commuting_out[1].second, 0.25);
        }
        {
                auto commuting_in  = sortLoc(location2->GetIncomingCommuningCities());
                auto commuting_out = sortLoc(location2->GetOutgoingCommuningCities());
                EXPECT_EQ(commuting_out.size(), 2);
                EXPECT_EQ(commuting_in.size(), 1);

                EXPECT_EQ(commuting_in[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.50);

                EXPECT_EQ(commuting_out[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_out[0].second, 0.75);
                EXPECT_EQ(commuting_out[1].first->GetID(), 3);
                EXPECT_DOUBLE_EQ(commuting_out[1].second, 0.5);
        }
        {
                auto commuting_in  = sortLoc(location3->GetIncomingCommuningCities());
                auto commuting_out = sortLoc(location3->GetOutgoingCommuningCities());
                EXPECT_EQ(commuting_out.size(), 0);
                EXPECT_EQ(commuting_in.size(), 2);

                EXPECT_EQ(commuting_in[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.25);
                EXPECT_EQ(commuting_in[1].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_in[1].second, 0.5);
        }
}

TEST(GeoGridJSONReaderTest, contactCentersTest)
{
        auto                        pop            = stride::Population::Create();
        auto                        geoGrid        = getGeoGridForFile("test1.json", pop.get());
        auto                        location       = geoGrid->Get(0);
        auto                        contactCenters = location->GetContactCenters();
        std::map<std::string, bool> found          = {{"K12School", false},
                                             {"Primary Community", false},
                                             {"College", false},
                                             {"Household", false},
                                             {"Workplace", false}};

        for (unsigned int i = 0; i < 5; i++) {
                EXPECT_FALSE(found[contactCenters[i]->GetType()]);
                found[contactCenters[i]->GetType()] = true;
        }
        for (auto& type : found) {
                EXPECT_TRUE(type.second);
        }
}

void runPeopleTest(std::string filename)
{
        auto                       pop      = stride::Population::Create();
        auto                       geoGrid  = getGeoGridForFile(filename, pop.get());
        auto                       location = geoGrid->Get(0);
        std::map<int, std::string> ids      = {{0, "K12School"}, {1, "Primary Community"}, {2, "Secondary Community"},
                                          {3, "College"},   {4, "Household"},         {5, "Workplace"}};

        EXPECT_EQ(location->GetID(), 1);
        EXPECT_EQ(location->GetName(), "Bavikhove");
        EXPECT_EQ(location->GetProvince(), 4);
        EXPECT_EQ(location->GetPopulation(), 2500);
        EXPECT_EQ(get<0>(location->GetCoordinate()), 0);
        EXPECT_EQ(get<1>(location->GetCoordinate()), 0);

        auto contactCenters = location->GetContactCenters();
        for (const auto& center : contactCenters) {
                auto pool   = center->GetPools()[0];
                auto person = *(pool->begin());
                EXPECT_EQ(ids[center->GetId()], center->GetType());
                EXPECT_EQ(person->GetId(), 1);
                EXPECT_EQ(person->GetAge(), 18);
                EXPECT_EQ(person->GetGender(), 'M');
                EXPECT_EQ(person->GetK12SchoolId(), 2);
                EXPECT_EQ(person->GetHouseholdId(), 5);
                EXPECT_EQ(person->GetCollegeId(), 4);
                EXPECT_EQ(person->GetWorkId(), 6);
                EXPECT_EQ(person->GetPrimaryCommunityId(), 3);
                EXPECT_EQ(person->GetSecondaryCommunityId(), 7);
        }
}

TEST(GeoGridJSONReaderTest, peopleTest) { runPeopleTest("test2.json"); }

TEST(GeoGridJSONReaderTest, intTest) { runPeopleTest("test3.json"); }

TEST(GeoGridJSONReaderTest, emptyStreamTest)
{
        auto              instream = std::make_unique<std::istringstream>("");
        auto              pop      = stride::Population::Create();
        GeoGridJSONReader geoGridJSONReader(std::move(instream), pop.get());
        EXPECT_THROW(geoGridJSONReader.Read(), stride::util::Exception);
}

TEST(GeoGridJSONReaderTest, invalidTypeTest)
{
        auto pop = stride::Population::Create();
        EXPECT_THROW(getGeoGridForFile("test4.json", pop.get()), stride::util::Exception);
}

TEST(GeoGridJSONReaderTest, invalidPersonTest)
{
        auto pop = stride::Population::Create();
        EXPECT_THROW(getGeoGridForFile("test5.json", pop.get()), stride::util::Exception);
}

TEST(GeoGridJSONReaderTest, invalidJSONTest)
{
        auto pop = stride::Population::Create();
        EXPECT_THROW(getGeoGridForFile("test6.json", pop.get()), stride::util::Exception);
}

} // namespace
