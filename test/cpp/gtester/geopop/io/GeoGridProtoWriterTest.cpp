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
#include "geopop/ContactCenter.h"
#include "geopop/GeoGridConfig.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;

namespace {

TEST(GeoGridProtoWriterTest, locationTest)
{
        const auto pop     = Population::Create();
        auto&      geoGrid = pop->RefGeoGrid();
        geoGrid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location>(3, 2, Coordinate(0, 0), "Mons", 2500));

        CompareGeoGrid(geoGrid);
}
TEST(GeoGridProtoWriterTest, contactCentersTest)
{
        const auto pop      = Population::Create();
        auto&      geoGrid  = pop->RefGeoGrid();
        const auto location = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);
        location->AddCenter(make_shared<ContactCenter>(0, Id::K12School));
        location->AddCenter(make_shared<ContactCenter>(1, Id::PrimaryCommunity));
        location->AddCenter(make_shared<ContactCenter>(2, Id::College));
        location->AddCenter(make_shared<ContactCenter>(3, Id::Household));
        location->AddCenter(make_shared<ContactCenter>(4, Id::Workplace));
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
