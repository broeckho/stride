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

#include "GeoGridIOUtils.h"

#include "geopop/College.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"
#include "geopop/K12School.h"
#include "geopop/PrimaryCommunity.h"
#include "geopop/Workplace.h"
#include "geopop/io/GeoGridJSONWriter.h"
#include "util/FileSys.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::util;
using boost::property_tree::ptree;

namespace {

void sortContactCenters(ptree& pt)
{
        auto& contactCenters       = pt.get_child("contactCenters");
        auto  compareContactCenter = [](const pair<string, ptree>& a, const pair<string, ptree>& b) {
                return a.second.get<string>("type") < b.second.get<string>("type");
        };
        contactCenters.sort<decltype(compareContactCenter)>(compareContactCenter);
}

void sortTree(ptree& tree)
{
        auto compareLocation = [](const pair<string, ptree>& a, const pair<string, ptree>& b) {
                return a.second.get<string>("id") < b.second.get<string>("id");
        };
        auto& locations = tree.get_child("locations");
        locations.sort<decltype(compareLocation)>(compareLocation);

        for (auto it = locations.begin(); it != locations.end(); it++) {
                sortContactCenters(it->second.get_child(""));
        }
}

bool compareGeoGrid(shared_ptr<GeoGrid> geoGrid, const string& testname)
{
        GeoGridJSONWriter writer;
        stringstream      ss;
        writer.Write(move(geoGrid), ss);
        ptree result;
        read_json(ss, result);
        sortTree(result);

        ptree expected;
        read_json(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/" + testname, expected);
        sortTree(expected);

        ostringstream oss1, oss2;
        boost::property_tree::xml_parser::write_xml(oss1, result);
        boost::property_tree::xml_parser::write_xml(oss2, expected);
        // return result == expected;
        return oss1.str() == oss2.str();
}

TEST(GeoGridJSONWriterTest, locationTest)
{
        auto pop     = Population::Create();
        auto geoGrid = make_shared<GeoGrid>(pop.get());
        geoGrid->AddLocation(make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove"));
        geoGrid->AddLocation(make_shared<Location>(2, 3, 5000, Coordinate(0, 0), "Gent"));
        geoGrid->AddLocation(make_shared<Location>(3, 2, 2500, Coordinate(0, 0), "Mons"));

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test0.json"));
}
TEST(GeoGridJSONWriterTest, contactCentersTest)
{
        auto pop      = Population::Create();
        auto geoGrid  = make_shared<GeoGrid>(pop.get());
        auto location = make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove");
        location->AddContactCenter(make_shared<K12School>(0));
        location->AddContactCenter(make_shared<PrimaryCommunity>(1));
        location->AddContactCenter(make_shared<College>(2));
        location->AddContactCenter(make_shared<Household>(3));
        location->AddContactCenter(make_shared<Workplace>(4));
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
