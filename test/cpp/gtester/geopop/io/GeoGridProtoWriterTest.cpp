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
#include "geopop/GeoGridConfig.h"
#include "pop/Population.h"

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

TEST(GeoGridProtoWriterTest, contactPoolsTest)
{
        const auto pop     = Population::Create();
        auto&      geoGrid = pop->RefGeoGrid();
        const auto loc     = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        loc->RefPools(Id::K12School).emplace_back(pop->RefPoolSys().CreateContactPool(Id::K12School));
        loc->RefPools(Id::PrimaryCommunity).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity));
        loc->RefPools(Id::College).emplace_back(pop->RefPoolSys().CreateContactPool(Id::College));
        loc->RefPools(Id::Household).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Household));
        loc->RefPools(Id::Workplace).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Workplace));
        geoGrid.AddLocation(loc);

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
