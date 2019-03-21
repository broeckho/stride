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
 *  Copyright 2019, Jan Broeckhove.
 */

#include "geopop/generators/PrimaryCommunityGenerator.h"

#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

TEST(PrimaryCommunityGeneratorTest, OneLocationTest)
{
        RnMan                       rnMan{RnInfo()};
        PrimaryCommunityGenerator   pcGenerator(rnMan);
        unsigned int                pcCounter{1U};

        GeoGridConfig               config{};
        config.input.pop_size = 10000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 2500);
        geoGrid.AddLocation(loc1);

        const auto& c1 = loc1->CRefCenters(Id::PrimaryCommunity);
        EXPECT_EQ(c1.size(), 0);

        const auto& p1 = loc1->CRefPools(Id::PrimaryCommunity);
        EXPECT_EQ(p1.size(), 0);

        pcGenerator.Apply(geoGrid, config, pcCounter);
        EXPECT_EQ(c1.size(), 5);
        EXPECT_EQ(p1.size(), 5 * config.pools.pools_per_primary_community);
}

TEST(PrimaryCommunityGeneratorTest, EqualLocationTest)
{
        RnMan                       rnMan{RnInfo()};
        PrimaryCommunityGenerator   pcGenerator(rnMan);
        unsigned int                pcCounter{1U};

        GeoGridConfig               config{};
        config.input.pop_size = 100 * 100 * 1000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        for (int i = 0; i < 10; i++) {
                geoGrid.AddLocation(
                    make_shared<Location>(1, 4, Coordinate(0, 0), "Location " + to_string(i), 10 * 1000 * 1000));
        }

        pcGenerator.Apply(geoGrid, config, pcCounter);

        vector<int> expectedCount{546, 495, 475, 500, 463, 533, 472, 539, 496, 481};
        for (int i = 0; i < 10; i++) {
                const auto& c1 = geoGrid[i]->CRefCenters(Id::PrimaryCommunity);
                EXPECT_EQ(expectedCount[i], c1.size());
        }
}

// Check can handle empty GeoGrid.
TEST(PrimaryCommunityGeneratorTest, ZeroLocationTest)
{
        RnMan                       rnMan{RnInfo()};
        PrimaryCommunityGenerator   pcGenerator(rnMan);
        unsigned int                pcCounter{1U};
        GeoGridConfig               config{};
        config.input.pop_size = 10000;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());

        pcGenerator.Apply(geoGrid, config, pcCounter);

        EXPECT_EQ(geoGrid.size(), 0);
}

// Check for five Locations.
TEST(PrimaryCommunityGeneratorTest, FiveLocationsTest)
{
        RnMan                       rnMan{RnInfo()};
        PrimaryCommunityGenerator   pcGenerator(rnMan);
        unsigned int                pcCounter{1U};

        GeoGridConfig               config{};

        IdSubscriptArray<unsigned int> contactCenterCounter(1U);

        config.input.pop_size             = 37542 * 100;
        config.popInfo.popcount_k12school = 750840;

        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        auto loc1    = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 10150 * 100);
        auto loc2    = make_shared<Location>(1, 4, Coordinate(0, 0), "Vlaams-Brabant", 10040 * 100);
        auto loc3    = make_shared<Location>(1, 4, Coordinate(0, 0), "Henegouwen", 7460 * 100);
        auto loc4    = make_shared<Location>(1, 4, Coordinate(0, 0), "Limburg", 3269 * 100);
        auto loc5    = make_shared<Location>(1, 4, Coordinate(0, 0), "Luxemburg", 4123 * 100);

        geoGrid.AddLocation(loc1);
        geoGrid.AddLocation(loc2);
        geoGrid.AddLocation(loc3);
        geoGrid.AddLocation(loc4);
        geoGrid.AddLocation(loc5);

        pcGenerator.Apply(geoGrid, config, pcCounter);

        vector<int> expectedCount{553, 518, 410, 173, 224};
        for (int i = 0; i < 5; i++) {
                const auto& cc = geoGrid[i]->CRefCenters(Id::PrimaryCommunity);
                EXPECT_EQ(expectedCount[i], cc.size());

                const auto& cp = geoGrid[i]->CRefPools(Id::PrimaryCommunity);
                EXPECT_EQ(expectedCount[i] * config.pools.pools_per_primary_community, cp.size());
        }
}

} // namespace
