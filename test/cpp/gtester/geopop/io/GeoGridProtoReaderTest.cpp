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
#include "geogrid.pb.h"
#include "geopop/CollegeCenter.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/K12SchoolCenter.h"
#include "geopop/PrimaryCommunityCenter.h"
#include "geopop/SecondaryCommunityCenter.h"
#include "geopop/WorkplaceCenter.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using boost::geometry::get;

namespace {

void fillLocation(int id, unsigned int province, unsigned int population, Coordinate coordinate, const string& name,
                  proto::GeoGrid_Location* location)
{
        location->set_id(id);
        location->set_province(province);
        location->set_name(name);
        location->set_population(population);
        const auto protoCoordinate = new proto::GeoGrid_Location_Coordinate();
        protoCoordinate->set_latitude(get<1>(coordinate));
        protoCoordinate->set_longitude(get<0>(coordinate));
        location->set_allocated_coordinate(protoCoordinate);
}

void fillContactCenter(const shared_ptr<ContactCenter>&       contactCenter,
                       proto::GeoGrid_Location_ContactCenter* protoContactCenter)
{
        using namespace stride::ContactType;

        map<Id, proto::GeoGrid_Location_ContactCenter_Type> types = {
            {Id::K12School, proto::GeoGrid_Location_ContactCenter_Type_K12School},
            {Id::PrimaryCommunity, proto::GeoGrid_Location_ContactCenter_Type_PrimaryCommunity},
            {Id::SecondaryCommunity, proto::GeoGrid_Location_ContactCenter_Type_SecondaryCommunity},
            {Id::College, proto::GeoGrid_Location_ContactCenter_Type_College},
            {Id::Household, proto::GeoGrid_Location_ContactCenter_Type_Household},
            {Id::Workplace, proto::GeoGrid_Location_ContactCenter_Type_Workplace}};

        protoContactCenter->set_type(types[contactCenter->GetContactPoolType()]);
        protoContactCenter->set_id(contactCenter->GetId());
}

TEST(GeoGridProtoReaderTest, locationTest)
{
        proto::GeoGrid geoGrid;
        fillLocation(1, 4, 2500, Coordinate(0, 0), "Bavikhove", geoGrid.add_locations());
        fillLocation(2, 3, 5000, Coordinate(0, 0), "Gent", geoGrid.add_locations());
        fillLocation(3, 2, 2500, Coordinate(0, 0), "Mons", geoGrid.add_locations());
        CompareGeoGrid(geoGrid);
}
TEST(GeoGridProtoReaderTest, contactCentersTest)
{
        proto::GeoGrid geoGrid;
        auto           location = geoGrid.add_locations();
        fillLocation(1, 4, 2500, Coordinate(0, 0), "Bavikhove", location);
        fillContactCenter(make_shared<K12SchoolCenter>(0), location->add_contactcenters());
        fillContactCenter(make_shared<PrimaryCommunityCenter>(1), location->add_contactcenters());
        fillContactCenter(make_shared<CollegeCenter>(2), location->add_contactcenters());
        fillContactCenter(make_shared<HouseholdCenter>(3), location->add_contactcenters());
        fillContactCenter(make_shared<WorkplaceCenter>(4), location->add_contactcenters());

        CompareGeoGrid(geoGrid);
}
TEST(GeoGridProtoReaderTest, peopleTest)
{
        proto::GeoGrid geoGrid;
        auto           location = geoGrid.add_locations();
        fillLocation(1, 4, 2500, Coordinate(0, 0), "Bavikhove", location);

        {
                auto contactCenter = location->add_contactcenters();
                fillContactCenter(make_shared<K12SchoolCenter>(0), contactCenter);
                auto pool = contactCenter->add_pools();
                pool->set_id(2);
                pool->add_people(1);
        }
        {
                auto contactCenter = location->add_contactcenters();
                fillContactCenter(make_shared<PrimaryCommunityCenter>(1), contactCenter);
                auto pool = contactCenter->add_pools();
                pool->set_id(3);
                pool->add_people(1);
        }
        {
                auto contactCenter = location->add_contactcenters();
                fillContactCenter(make_shared<SecondaryCommunityCenter>(2), contactCenter);
                auto pool = contactCenter->add_pools();
                pool->set_id(7);
                pool->add_people(1);
        }
        {
                auto contactCenter = location->add_contactcenters();
                fillContactCenter(make_shared<CollegeCenter>(3), contactCenter);
                auto pool = contactCenter->add_pools();
                pool->set_id(4);
                pool->add_people(1);
        }
        {
                auto contactCenter = location->add_contactcenters();
                fillContactCenter(make_shared<HouseholdCenter>(4), contactCenter);
                auto pool = contactCenter->add_pools();
                pool->set_id(5);
                pool->add_people(1);
        }
        {
                auto contactCenter = location->add_contactcenters();
                fillContactCenter(make_shared<WorkplaceCenter>(5), contactCenter);
                auto pool = contactCenter->add_pools();
                pool->set_id(6);
                pool->add_people(1);
        }
        auto person = geoGrid.add_persons();
        person->set_id(1);
        person->set_age(18);

        CompareGeoGrid(geoGrid);
}

TEST(GeoGridProtoReaderTest, commutesTest)
{
        proto::GeoGrid geoGrid;
        auto           bavikhove = geoGrid.add_locations();
        auto           gent      = geoGrid.add_locations();
        auto           mons      = geoGrid.add_locations();
        fillLocation(1, 4, 2500, Coordinate(0, 0), "Bavikhove", bavikhove);
        fillLocation(2, 4, 2500, Coordinate(0, 0), "Gent", gent);
        fillLocation(3, 4, 2500, Coordinate(0, 0), "Mons", mons);
        {
                auto commute = bavikhove->add_commutes();
                commute->set_to(gent->id());
                commute->set_proportion(0.5);
        }
        {
                auto commute = bavikhove->add_commutes();
                commute->set_to(mons->id());
                commute->set_proportion(0.25);
        }
        {
                auto commute = gent->add_commutes();
                commute->set_to(bavikhove->id());
                commute->set_proportion(0.75);
        }
        {
                auto commute = gent->add_commutes();
                commute->set_to(mons->id());
                commute->set_proportion(0.5);
        }

        CompareGeoGrid(geoGrid);
}
} // namespace
