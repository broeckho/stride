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
#include "gengeopop/SecondaryCommunity.h"
#include "gengeopop/Workplace.h"
#include "gengeopop/generators/GeoGridGenerator.h"
#include "gengeopop/io/GeoGridProtoWriter.h"
#include "gengeopop/io/proto/geogrid.pb.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>

using namespace std;
using namespace gengeopop;
using namespace stride;

namespace {

shared_ptr<GeoGrid> GetGeoGrid(Population* pop)
{
        GeoGridConfig config{};
        config.input.populationSize        = 10000;
        config.calculated.compulsoryPupils = static_cast<unsigned int>(0.20 * 1000);

        GeoGridGenerator geoGridGenerator(config, make_shared<GeoGrid>(pop));
        return geoGridGenerator.GetGeoGrid();
}

TEST(GeoGridProtoWriterTest, locationTest)
{
        auto pop = Population::Create();
        const auto geoGrid = GetGeoGrid(pop.get());
        geoGrid->AddLocation(make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove"));
        geoGrid->AddLocation(make_shared<Location>(2, 3, 5000, Coordinate(0, 0), "Gent"));
        geoGrid->AddLocation(make_shared<Location>(3, 2, 2500, Coordinate(0, 0), "Mons"));

        CompareGeoGrid(geoGrid);
}
TEST(GeoGridProtoWriterTest, contactCentersTest)
{
        auto pop = Population::Create();
        const auto geoGrid  = GetGeoGrid(pop.get());
        const auto location = make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove");
        location->AddContactCenter(make_shared<K12School>(0));
        location->AddContactCenter(make_shared<PrimaryCommunity>(1));
        location->AddContactCenter(make_shared<College>(2));
        location->AddContactCenter(make_shared<Household>(3));
        location->AddContactCenter(make_shared<Workplace>(4));
        geoGrid->AddLocation(location);

        CompareGeoGrid(geoGrid);
}

TEST(GeoGridProtoWriterTest, peopleTest)
{
        auto pop = Population::Create();
        CompareGeoGrid(GetPopulatedGeoGrid(pop.get()));
}

TEST(GeoGridProtoWriterTest, commutesTest)
{
        auto pop = Population::Create();
        CompareGeoGrid(GetCommutesGeoGrid(pop.get()));
}

} // namespace
