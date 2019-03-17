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

#include "geopop/io/GeoGridProtoWriter.h"
#include "GeoGridIOUtils.h"
#include "geopop/College.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"
#include "geopop/K12School.h"
#include "geopop/PrimaryCommunity.h"
#include "geopop/SecondaryCommunity.h"
#include "geopop/Workplace.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;

namespace {

TEST(GeoGridProtoWriterTest, locationTest)
{
        const auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        geoGrid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location>(3, 2, Coordinate(0, 0), "Mons", 2500));

        CompareGeoGrid(geoGrid);
}
TEST(GeoGridProtoWriterTest, contactCentersTest)
{
        const auto pop      = Population::Create();
        auto geoGrid  = GeoGrid(pop.get());
        const auto location = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);
        location->AddCenter(make_shared<K12School>(0));
        location->AddCenter(make_shared<PrimaryCommunity>(1));
        location->AddCenter(make_shared<College>(2));
        location->AddCenter(make_shared<Household>(3));
        location->AddCenter(make_shared<Workplace>(4));
        geoGrid.AddLocation(location);

        CompareGeoGrid(geoGrid);
}

TEST(GeoGridProtoWriterTest, peopleTest)
{
        auto pop = Population::Create();
        CompareGeoGrid(*GetPopulatedGeoGrid(pop.get()));
}

TEST(GeoGridProtoWriterTest, commutesTest)
{
        auto pop = Population::Create();
        CompareGeoGrid(*GetCommutesGeoGrid(pop.get()));
}

} // namespace
