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

#include "GeoGridIOUtils.h"

#include "gengeopop/College.h"
#include "gengeopop/Community.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/K12School.h"
#include "gengeopop/PrimaryCommunity.h"
#include "gengeopop/Workplace.h"
#include "gengeopop/generators/GeoGridGenerator.h"
#include "gengeopop/io/GeoGridJSONWriter.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>

using namespace std;
using namespace gengeopop;
using namespace stride;
using namespace stride::util;
using boost::property_tree::ptree;

namespace {

void sortContactCenters(ptree& pt)
{
        auto& contactCenters      = pt.get_child("contactCenters");
        auto compareContactCenter = [](const std::pair<std::string, ptree>& a, const std::pair<std::string, ptree>& b) {
                return a.second.get<std::string>("type") < b.second.get<std::string>("type");
        };
        contactCenters.sort<decltype(compareContactCenter)>(compareContactCenter);
}

void sortTree(ptree& tree)
{
        auto compareLocation = [](const std::pair<std::string, ptree>& a, const std::pair<std::string, ptree>& b) {
                return a.second.get<std::string>("id") < b.second.get<std::string>("id");
        };
        auto& locations = tree.get_child("locations");
        locations.sort<decltype(compareLocation)>(compareLocation);

        for (auto it = locations.begin(); it != locations.end(); it++) {
                sortContactCenters(it->second.get_child(""));
        }
}

bool compareGeoGrid(std::shared_ptr<GeoGrid> geoGrid, const std::string& testname)
{
        GeoGridJSONWriter writer;
        std::stringstream ss;
        writer.Write(std::move(geoGrid), ss);
        ptree result;
        read_json(ss, result);
        sortTree(result);

        ptree expected;
        read_json(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/" + testname, expected);
        sortTree(expected);

        return result == expected;
}

TEST(GeoGridJSONWriterTest, locationTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GetGeoGrid(pop.get());
        geoGrid->AddLocation(std::make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove"));
        geoGrid->AddLocation(std::make_shared<Location>(2, 3, 5000, Coordinate(0, 0), "Gent"));
        geoGrid->AddLocation(std::make_shared<Location>(3, 2, 2500, Coordinate(0, 0), "Mons"));

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test0.json"));
}
TEST(GeoGridJSONWriterTest, contactCentersTest)
{
        auto pop      = Population::Create();
        auto geoGrid  = GetGeoGrid(pop.get());
        auto location = std::make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove");
        location->AddContactCenter(std::make_shared<K12School>(0));
        location->AddContactCenter(std::make_shared<PrimaryCommunity>(1));
        location->AddContactCenter(std::make_shared<College>(2));
        location->AddContactCenter(std::make_shared<Household>(3));
        location->AddContactCenter(std::make_shared<Workplace>(4));
        geoGrid->AddLocation(location);

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test1.json"));
}

TEST(GeoGridJSONWriterTest, peopleTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(GetPopulatedGeoGrid(pop.get()), "test2.json"));
}

TEST(GeoGridJSONWriterTest, commutesTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(GetCommutesGeoGrid(pop.get()), "test7.json"));
}

} // namespace
